# godot-ndi
This implements a node that will send the viewport nearest to it as an NDI® feed.

NDI® is this https://ndi.video/
(NDI® is a registered trademark of Vizrt NDI AB)
## What works now

This works in linux 86/64. Just make a GDNDI node in your scene tree, near the viewport you want to expose and you should get an ndi feed.

Right now it will be called `GODOT NDI PLZ` and there's nothing you can do about it. The resolution of the feed will change as your game screen resolution changes.

As long as there is less than 6144*6144 pixels total in your game image, it should work just fine.
