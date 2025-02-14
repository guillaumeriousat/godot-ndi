# godot-ndi

While I was working (very slowly) on this, someone else had the time to start and mostly finish something that is more polished and cross-platform.

You can find their project here : https://github.com/unvermuthet/godot-ndi

I recommend that you use that instead. I'll be keeping this here as archive and example code.




This implements a node that will send the viewport nearest to it as an NDI® feed.

NDI® is this https://ndi.video/
(NDI® is a registered trademark of Vizrt NDI AB)
## What works now

This works in linux 86/64. Just make a GDNDI node in your scene tree, near the viewport you want to expose and you should get an ndi feed.

You can rename the feed with the `stream name` property. The resolution of the feed will change as your game screen resolution changes.

As long as there is less than 6144*6144 pixels total in your game image, it should work just fine.
