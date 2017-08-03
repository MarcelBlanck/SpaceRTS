# SpaceRTS
Master Thesis - A space themed RTS for Samsung Gear VR

I will write a bit more later and eventually even upload my Thesis on my website.

For my Master Thesis I started to build a Space RPG like Homeworld for the back then brand new Gear VR for the Samsung Galaxy Note 4.

Unreal Engine let me down back then and did not fullfill their feature plan, so I had to do a lot of things for the Gear VR integaton myself, like the input methods. Nowadays everything in UE4 is smooth and totally ready for any VR development, but back then in 2015 it was a rough ride.

So instead of getting as far as I wanted I had to implement a lot of stuff fpr Gear VR support first, the imput methods were totalyl not available. https://github.com/MarcelBlanck/SpaceRTS/tree/master/Source/SpaceRTS/GearVR

One of the most interesting parts was the implementation of the "Reciprocal Velocity Obstacles for Real-Time Multi-Agent Navigation" http://gamma.cs.unc.edu/RVO/ and intigrating it in the UE4 https://github.com/MarcelBlanck/SpaceRTS/tree/master/Source/SpaceRTS/Steering

Additionally it was neccesary to find tricks to get a stable framerate of 60 with a lots of ships maneuvering so the Level Blueprint does the scheduling for all the ships.

I also created a basic Gaze gui implementation, since again UE4 was late with UMG VR Support. https://github.com/MarcelBlanck/SpaceRTS/tree/master/Source/SpaceRTS/GUI

A lot of interesting stuff happens in Blueprint too, but to look at that you would need to launch the software in the editor.


If this code helped you in any way or inspired you, let me know =)
