using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Drawing;

using GTA;
using GTA.Math;

namespace PlaneBomb
{
    public class Hello
    {
        public Hello(string animalName)
        {
            mAnimalName = animalName;
        }

        public void DoSomething()
        {
            if (mAnimalName == "cat")
            {
                Console.WriteLine(text);
            }
            else if (mAnimalName == "dog")
            {

            }
        }

        public string ReturnValue()
        {
            text = text + " there";
            return text;
        }

        string text = "hi";
        string mAnimalName = "";
    }



    public class PlaneBomb : Script
    {
        public PlaneBomb()
        {
            Tick += OnTick;
            KeyDown += OnKeyDown;
        }
        

        void OnKeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F8)
            {
                SummonPlane();
            }
        }

        bool Done = false;
        Vehicle plane;
        Stopwatch explodeTimer;
        Stopwatch explodeInterval;

        Vector3 Target;

        bool isExploding = false;

        void OnTick(object sender, EventArgs e)
        {
            Ped player = Game.Player.Character;

            if (!player.IsDead)
            {
                if (plane != null && plane.Exists())
                {
                    CheckPlane();
                }
                if (isExploding && plane != null && plane.Exists())
                {
                    Explosion();
                }
                
                var target = GameplayCamera.Rotation;
                var dist = Math.Tan(Math.Abs(target.X)) * GameplayCamera.Position.Z;

                //lerp until we hit the ground. this is the target location
                for (int i = 0; i < 50; i++)
                {
                    var nowVector = Vector3.Lerp(GameplayCamera.Position, Vector3.Multiply(Vector3.Normalize(target), 2000), (i / 100));
                    if (nowVector.Z - World.GetGroundZ(nowVector) <= 0)
                    {
                        Target = nowVector;
                        
                        break;
                    }
                }

                //var text = new UIText(GameplayCamera.Rotation.ToString(), new Point(20, 300), .4f, Color.Red);
                //var text2 = new UIText(GameplayCamera.Position.Z.ToString(), new Point(20, 275), .4f, Color.Red);
                //var text3 = new UIText(Target.ToString(), new Point(20, 250), .4f, Color.Red);

                //text.Draw();
                //text2.Draw();
                //text3.Draw();
            }

        }

        void DrawMarker()
        {
            Ped player = Game.Player.Character;

            var target = GameplayCamera.Rotation;

            var zval = World.GetGroundZ(target);

            Vector3 vector = new Vector3(target.X,target.Y,zval);

            var blip = World.CreateBlip(vector);
        }

        private void SummonPlane()
        {
            Ped player = Game.Player.Character;

            Hello this_thing = new Hello("cat");

            this_thing.DoSomething();
            string value = this_thing.ReturnValue();


            var target = GameplayCamera.Rotation;
            var dist = Math.Tan(Math.Abs(target.X)) * GameplayCamera.Position.Z;

            //lerp until we hit the ground. this is the target location
            for(int i = 0; i < 50; i++)
            {
                var nowVector = Vector3.Lerp(GameplayCamera.Position, Vector3.Multiply(Vector3.Normalize(target),2000),(i / 100));
                if (nowVector.Z  - World.GetGroundZ(nowVector) <= 0)
                {
                    Target = nowVector;
                    break;
                }
            }

            if (Target == null)
            {
                return;
            }

            var spawnHeight = GameplayCamera.Position.Z + 200;

            float rot = (float)(Math.Atan(dist / spawnHeight));
            rot = -(90 - rot);

            Model jet = new Model(GTA.Native.VehicleHash.Hydra);

            Vector3 pos = new Vector3(player.Position.X, player.Position.Y, spawnHeight);

            var thing = new Vector3(rot, 0, player.Rotation.Z);

            plane = World.CreateVehicle(jet, pos, player.Heading);
            //Vector3 lookRotation = (target - plane.Position);
            //lookRotation.Normalize();

            plane.Rotation = thing;
            plane.Speed = 100;
            
            Done = true;
        }

        void CheckPlane()
        {
            Ped player = Game.Player.Character;
            try
            {
                if (!plane.IsDriveable && plane.Exists() && plane != null)
                {
                    //Debug();
                    isExploding = true;
                    plane.Explode();
                    World.AddOwnedExplosion(player, plane.Position, ExplosionType.BigExplosion1, 75, 1);
                    World.AddOwnedExplosion(player, plane.Position, ExplosionType.ExplosionWithFire4, 100, 2);
                    explodeTimer.Start();
                    explodeInterval.Start();
                }
            }
            catch
            { }

        }

        void Explosion()
        {
            Ped player = Game.Player.Character;
            var gen = new Random();
            var ran = gen.NextDouble();
            float random = (float)ran * 5;
            var ground = World.GetGroundZ(plane.Position);

            Vector3 explodePos = new Vector3(plane.Position.X + random, plane.Position.Y + random, ground);
            
            if (explodeTimer.ElapsedMilliseconds < (2 * 1000) && explodeInterval.ElapsedMilliseconds > (0.25 * 1000))
            {
                World.AddOwnedExplosion(player, explodePos, ExplosionType.BigExplosion1, 200, 1);
                explodeInterval.Reset();
            }
            else
            {
                explodeTimer.Stop();
                explodeTimer.Reset();

                explodeInterval.Reset();
                explodeInterval.Stop();

                isExploding = false;
            }
        }

        void  Debug()
        {
            List<UIText> hud = new List<UIText>();

            hud.Add(MakeText("Forward Vector: " + plane.ForwardVector.ToString(), new Point(20, 325)));
            hud.Add(MakeText("Up Vector: " + plane.UpVector.ToString(), new Point(20, 350)));
            hud.Add(MakeText("Right Vector: " + plane.RightVector.ToString(), new Point(20, 375)));
            hud.Add(MakeText("Heading: " + plane.Heading.ToString(), new Point(20, 400)));
            hud.Add(MakeText("HeightAboveGround: " + plane.HeightAboveGround.ToString(), new Point(20, 425)));
            hud.Add(MakeText("Rotation: " + plane.Rotation.ToString(), new Point(20, 450)));
            hud.Add(MakeText("Health: " + plane.Health.ToString(), new Point(20, 475)));
            hud.Add(MakeText("Velocity: " + plane.Velocity.ToString(), new Point(20, 500)));
            hud.Add(MakeText("Speed: " + plane.Speed.ToString(), new Point(20, 525)));


            foreach (UIText text in hud)
            {
                text.Draw();
            }
            var point = UI.WorldToScreen(Target);
            var rect = new UIRectangle(point, new Size(2, 2), Color.Red);
            rect.Draw();


        }


        UIText MakeText(string text, Point point)
        {
            return new UIText(text, point, .35f, Color.Red);
        }
    }
}
