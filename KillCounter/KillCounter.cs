using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Threading;

using System.Diagnostics;

using GTA;
using GTA.Math;

namespace Killcounter
{
    public class KillCounter : Script
    {
        public KillCounter()
        {
            Tick += OnTick;
        }

        //list of all dead peds
        private List<int> DeadEntityList = new List<int>();
        //list of all tracked peds
        private List<int> EntityList = new List<int>();

        public int PedKillstreakLevel = 0;
        public int VehKillstreakLevel = 0;

        public int VehKillsNeeded = 20;
        public int PedKillsNeeded = 20;

        private int PedTally = 0;
        private int VehTally = 0;

        //fired when a kill occurs to flash the ui
        private bool PedZap = false;
        private bool VehZap = false;

        private bool PedTierZap = false;
        private bool VehTierZap = false;

        bool Killstreak = false;

        Ped Player;

        Stopwatch mTimer = new Stopwatch();

        //number of seconds before killstreak resets
        private int Cooldown = 15;
        //radius of area to check and qualify kills for the counter
        float Radius = 200000;

        /// <summary>
        /// on each game tick, perform this main loop
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void OnTick(object sender, EventArgs e)
        {
            Player = Game.Player.Character;
            
            MaintainLists();
            ScoutLists();
            UpdateDeadPeds();
            UpdateDeadVehicles();
            CheckStreak();
            TrackPerks();

            //every tick update UI if a killstreak is live
            if (Killstreak)
            {
                UpdateUI();
            }
        }

        //update player perks
        void TrackPerks()
        {
            switch (PedKillstreakLevel)
            {
                case 0:
                    break;
                case 1:
                    break;
            } 
            switch (VehKillstreakLevel)
            {
                case 0:
                    break;
                case 1:
                    break;
            }
        }
        //scout around for extra targets
        //this is only required because there is not support for directly accessing global entity pool
        //pull entity that player is looking at and add to list if not already
        //do this by raycasting in direction of player
        void ScoutLists()
        {
            Vector3 rayDest = RotationToDirection(GameplayCamera.Rotation) * 50000;
            var results = World.Raycast(GameplayCamera.Position, rayDest, IntersectOptions.PedsAndVehicles);
            if (results.DitHitEntity)
            {
                //check if this hit entity is already in the pool
                if (!EntityList.Contains(results.HitEntity.Handle) && Entity.Exists(results.HitEntity))
                {
                    EntityList.Add(results.HitEntity.Handle);
                }
            }
        }
        private static Vector3 RotationToDirection(Vector3 Rotation)
        {
            Vector3 v;
            float z = Rotation.Z;
            float num = z * 0.0174532924f;
            float x = Rotation.X;
            float num2 = x * 0.0174532924f;
            float num3 = Math.Abs((float)Math.Cos((double)num2));
            v.X = (float)((double)((float)(-(float)Math.Sin((double)num))) * (double)num3);
            v.Y = (float)((double)((float)Math.Cos((double)num)) * (double)num3);
            v.Z = (float)Math.Sin((double)num2);
            return v;
        }
        void MaintainLists()
        {
            //poll for nearby
            var peds = World.GetNearbyPeds(Player, Radius);
            foreach (Ped ped in peds)
            {
                if (!EntityList.Contains(ped.Handle))
                {
                    EntityList.Add(ped.Handle);
                }
            }
            var vehs = World.GetNearbyVehicles(Player, Radius);
            foreach (Vehicle veh in vehs)
            {
                if (!EntityList.Contains(veh.Handle))
                {
                    EntityList.Add(veh.Handle);
                }
            }

            //cleanup lists of dead and tracked alive entities
            //remove if it no longer exists
            EntityList.RemoveAll(i => !Entity.Exists(new Ped(i)));
            DeadEntityList.RemoveAll(i => !Entity.Exists(new Ped(i)));
        }

        /// <summary>
        /// check for and update dead pedestrians
        /// </summary>
        void UpdateDeadPeds()
        {
            //for each entity in radius
            for (int i = 0; i < EntityList.Count - 1; i++)
            {
                Ped newped = new Ped(i);
                //check if its dead and damaged by us but not added previously
                if (newped.Exists() && newped.HasBeenDamagedBy(Player) && newped.IsDead)
                {
                    //check if we have added this dead ped yet
                    if (!DeadEntityList.Contains(newped.Handle))
                    {
                        PedTally = PedTally + 1;
                        DeadEntityList.Add(newped.Handle);
                        ResetCooldown();
                        PedZap = true;
                    }
                }
            }
        }

        /// <summary>
        /// check for and update dead vehicles
        /// </summary>
        void UpdateDeadVehicles()
        {            
            //for each entity in radius
            for (int i = 0; i < EntityList.Count - 1; i++)
            {
                Vehicle newveh = new Vehicle(i);
                //check if its dead and damaged by us but not added previously
                if (newveh.Exists() && newveh.HasBeenDamagedBy(Player) && !newveh.IsDriveable)
                {
                    //check if we have added this dead veh yet
                    if (!DeadEntityList.Contains(newveh.GetHashCode()))
                    {                        
                        VehTally = VehTally + 1;
                        DeadEntityList.Add(newveh.Handle);
                        ResetCooldown();
                        VehZap = true;
                    }
                }
            }
        }

        //check if we still qualify as a killstreak
        void CheckStreak()
        {
            //conditions for killstreak to go away:
            //time elapsed without kill  
            //player is dead
            if (mTimer.ElapsedMilliseconds > (Cooldown * 1000) || Player.IsDead)
            {
                Killstreak = false;
                VehTally = 0;
                PedTally = 0;
                mTimer.Stop();
                mTimer.Reset();
                VehKillstreakLevel=0;
                PedKillstreakLevel=0;
            }

            //check if the killstreak level has increased
            if (VehTally >= (VehKillstreakLevel + 1) * VehKillsNeeded )
            {
                VehKillstreakLevel++;
                VehTierZap = true;
            }

            if (PedTally >= (PedKillstreakLevel + 1) * PedKillsNeeded)
            {
                PedKillstreakLevel++;
                PedTierZap = true;
            }
        }

        //a kill has occurred, reset the timer
        void ResetCooldown()
        {
            //reset the timer if its running, to continue the streak
            if (mTimer.IsRunning)
            {
                mTimer.Stop();
                mTimer.Reset();
                mTimer.Start();
            }
            else
            {
                //if it is the first kill, start timer
                mTimer.Start();
            }
            Killstreak = true;
        }

        //draw the UI
        private void UpdateUI()
        {
            //dimensions of eligible UI space to draw in
            int y = UI.HEIGHT;
            int x = UI.WIDTH;
            //adjusted x and y coords for where to draw the menus (for varying resolutions)
            int render_x = (int)Math.Round(x * 0.025);
            int render_y = (int)Math.Round(y * .25);
            //gap between text
            int gap = (int)(y * .035);

            //Killstreak + countdown text:
            //remaining time on killstreak
            decimal remaining = Math.Round((decimal)Cooldown - (decimal)(mTimer.ElapsedMilliseconds / 1000m), 2);

            //if less than 0, dont ever display a negative number
            if (remaining < 0)
            {
                remaining = 0;
            }

            string captionTimer = "Killstreak: " + remaining.ToString();

            var textTimer = new UIText(captionTimer, new Point(render_x, render_y), .5f, Color.WhiteSmoke, GTA.Font.Pricedown, false);
            var textTimerBorder = new UIText(captionTimer, new Point(render_x - 1, render_y + 1), .51f, Color.Black, GTA.Font.Pricedown, false);

            //pedestrian UI text:
            var pedColor = Color.Ivory;
            if (PedZap)
            {
                pedColor = Color.Red;
                PedZap = false;
            }
            string captionPed = "Pedestrians Killed: " + PedTally.ToString();
            var textPed = new UIText(captionPed, new Point(render_x, render_y + gap), .4f, pedColor, GTA.Font.Pricedown, false);
            var textPedBorder = new UIText(captionPed, new Point(render_x - 1, render_y + gap + 1), .41f, Color.Black, GTA.Font.Pricedown, false);

            //vehicles UI text:
            var vehColor = Color.Ivory;
            if (VehZap)
            {
                vehColor = Color.Red;
                VehZap = false;
            }
            string captionVeh = "Vehicles Killed: " + VehTally.ToString();
            var textVeh = new UIText(captionVeh, new Point(render_x, render_y + gap + 20), .4f, vehColor, GTA.Font.Pricedown, false);
            var textVehBorder = new UIText(captionVeh, new Point(render_x - 1, render_y + gap + 20 + 1), .41f, Color.Black, GTA.Font.Pricedown, false);


            //killstreak levels text
            if (PedKillstreakLevel > 0)
            {
                //Ped Kill Tier UI text:
                var pedColorTier = Color.Ivory;
                if (PedTierZap)
                {
                    pedColorTier = Color.Red;
                    PedTierZap = false;
                }
                string captionPedTier = "Pedestrian Streak: Tier " + PedKillstreakLevel.ToString();
                var textPedTier = new UIText(captionPedTier, new Point(render_x, render_y + gap + 40), .4f, pedColorTier, GTA.Font.Pricedown, false);
                var textPedBorderTier = new UIText(captionPedTier, new Point(render_x - 1, render_y + gap + 20 + 1), .41f, Color.Black, GTA.Font.Pricedown, false);

                textPedBorderTier.Draw();
                textPedTier.Draw();
            }

            if (VehKillstreakLevel > 0)
            {
                //Veh kill tier UI text:
                var vehColorTier = Color.Ivory;
                if (VehTierZap)
                {
                    vehColorTier = Color.Red;
                    VehTierZap = false;
                }
                string captionVehTier = "Vehicle Streak: Tier " + VehKillstreakLevel.ToString();
                var textVehTier = new UIText(captionVehTier, new Point(render_x, render_y + gap + 60), .4f, vehColorTier, GTA.Font.Pricedown, false);
                var textVehBorderTier = new UIText(captionVehTier, new Point(render_x - 1, render_y + gap + 20 + 1), .41f, Color.Black, GTA.Font.Pricedown, false);

                textVehBorderTier.Draw();
                textVehTier.Draw();
            }

            textTimerBorder.Draw();
            textTimer.Draw();

            textPedBorder.Draw();
            textPed.Draw();

            textVehBorder.Draw();
            textVeh.Draw();
        }
    }
}