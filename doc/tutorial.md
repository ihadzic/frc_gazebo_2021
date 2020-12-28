# Introduction

With COVID lockdowns and restrictions on the number of students who can attend
the robotics club meetings in person, having a good simulation environment
has become an imperative. A good simulator is always a useful tool because
it allows code development while the robot is still being designed and
built. However, it is even more important when a large number of team
members are working from home.

## Simulator options and selection

FRC allows several simulation options, each having its own pros and cons.
They are Synthesis from AutoDesk, Gazebo from Open-Source Robotics Foundation,
and WPILib integrated simulator. The first two simulators do physics simulation
and also graphically render the robot moving and acting in its world. The
WPILib integrated simulator is more like an extension to the code debugger.

In our simulation, we will use a combination of WPILib integrated simulator
and Gazebo. There are several reasons why we picked Gazebo over synthesis.
Originally, Gazebo support for FRC, through [FRCSim](https://first.wpi.edu/FRC/roborio/release/simulation/) project has slowed down after
year 2016 and completely stopped afterwards to be replaced with Synthesis.
However, based on few recent commits in WPILib source code, it seems like
there is an intent to revive the Gazebo support.

Being an open tool with large number of existing models supported and fully
open specification and source code, Gazebo gives us a better control of the
robot model than Synthesis. In Gazebo, we have full control of the robot
model through a well defined and open SDF file and Gazebo plugins, so we
can tell exactly what the model is capturing and that it is not and if we
need to improve the model accuracy, we can.

In Synthesis, it is somewhat unclear what the process of exporting the robot
design from Inventor to Synthesis is doing and how much of the physical
properties the model is capturing. The format is proprietary, so it is
difficult to analyze and extend it. Further, the process of exporting the
robot requires a completed CAD design, which is typically late in the design
process. In Gazebo, SDF file can be developed in parallel with the robot
design. Many details of the robot CAD design do not need to be captured in
SDF file (or can be added later as the model becomes more detailed). For
example, to start developing the code that handles robot mobility (e.g.
for driving and autonomous operation), we only need a box on wheels with
inertial, friction, and slipping properties done captured "about right",
which can be easily put together by editing an SDF file. More details can
be added later and the file can be revised based on the progress of the
CAD design.

The downside of Gazebo is that it only works on Linux, just as the
downside of Synthesis is that it only works on Windows (Synthesis for
driving only works on Linux, but code emulation does not). Another downside
of Gazebo is that at present only C++ development is supported (the Gazebo
HAL extension builds for C++ only), but we can expect that Java support
will come soon (original FRCSim project supported it).

# Background

Gazebo simulator consists of two major components. The server, called
`gzserver`, is a physics-solver that simulates physical interaction of
objects (robots, field surface, field elements, etc.). The client, called
`gzclient`, is a graphics renderer that generates 3D scene representing
the robot and its surroundings. When you run Gazebo using `gazebo` command
from your shell, the client and server will start automatically.

To use Gazebo efficiently, it is necessary to understand some basic concepts
of how the tool works and how it interacts with external programs, namely the
FRC WPILib Hardware Abstraction Layer (HAL).

To learn Gazebo, you should go through all tutorials available from the
[Gazebo web site](http://gazebosim.org/tutorials). Here we only briefly
touch the basic concepts and refer to tutorials for more details.

## Simulation Description File (SDF)

The basic building block of Gazebo physical model is the Simulation Description
File (SDF). This file describes the structure of all simulation "protagonists",
which include robots, static objects like walls and obstacles, and dynamic
objects like game elements. The SDF file also describes the physical properties
of the simulation object, such as mass, moment of inertia, friction
coefficients, elasticity, damping factor, and many more. Getting physical
properties right is essential for accurate simulation, but to just get
something moving approximate modeling is often sufficient. A detailed model
can very accurately capture the behavior of a real-world robot, but it
takes time and a lot of knowledge of physics to develop it. A less detailed
model can get you going and allow you to verify basic functionality of the
robot which is sometimes sufficient.

You are in control of the level of detail and simulation accuracy and often
modeling and programming using the model is the activity that takes place
together with the robot CAD design.

SDF is an open specification that is based on XML language, which in turn
is very similar to HTML, a language used to describe the layout of a
web page. The [SDF Specification](http://sdformat.org/spec) is very detailed
and publicly available. Trying to read it without any context is probably
very dry and boring, but once you start building your own model, you will
be referring to it frequently to understand what properties are available
and what they mean.

### Links and Joints

The main components of an SDF file are links and joints. Links represent
the geometric components of the robot. For example a wheel is the link
of the type cylinder. A very simple robot chassis could be a rectangular
prism. Links are often associated with physical properties. The simplest
one to understand even with minimal knowledge of physics is the mass.
If you build a robot chassis and decide to model it as a rectangular prism,
you will probably want to annotate the link that represents it with the
information how heavy it is. When Gazebo simulates your robot
it will eventually get to solve Newton's equations to decide how force
applied to your robot will move it. Other more advanced properties (for
which you need more in-depth understanding of physics) would describe how
the link acts on collision or how it interacts with other surfaces it may
contact.

Joints are the elements that connect the links. For example if you want
to model a simple four-wheel robot you would define five links, one rectangular
prism for a chassis and four cylinders to model the wheels. To model mounting
of the wheel to the robot chassis, you would need a joint that represents
an axle. In this specific case, the joint would be of the type "revolute",
which is the joint that allows another link to spin around it. Joints
can also be assigned many physical properties that make their behavior in
simulation realistic. For example any real-world axle would have a finite
damping (due to friction), so that when the torque is no longer applied to
it the axle would slow down until it eventually stops rotating (remember
that, according to the law of inertia, the axle without any friction
would continue to spin forever after the torque is no longer applied).

So, in Gazebo, your robot is a collection links that are interconnected
with joints. When you get to building your robot model, you will be
editing the SDF file and working together with your design team
to capture all properties you need for a good-enough model.

## Meshes

In addition to physical and geometric properties, you may also want to
make the robot look pretty (and more realistic) in your simulation.
A simple robot consisting of the chassis and four wheels does not look
any better than the
[Flintstones Mobile](https://flintstones.fandom.com/wiki/Flintmobile),
although it is often good enough to reproduce the physical effects you
are trying to model. While, in theory, you could model every nut
and bolt on your robot using links and joint and even assign realistic
colors and lighting properties to them, this is often unnecessary.

A quick shortcut to making the robot look pretty is to take the visual
model from your CAD tool (or any 3D model design tool) and associate it
with the link. This is called mesh. The mesh only influences how the robot
looks in simulation, but it makes no difference on physics being simulated.

SDF file has a special tag that you can add to the link and point it to
the mesh file that is then used to visualize the robot. If you don't have
a mesh, the simulator (`gzclient`) will simply display the geometric objects
that represent your links. If you have a mesh file, the simulator will
display the mesh instead. The mesh is also a file that can be in Collada
or STL format. If you are familiar with 3D printing or if you have done any
3D design in Blender, you will probably recognize these file formats.

## World

Once you have all SDF files, such as your robot, field elements, obstacles,
field surface, etc., you need to tell Gazebo how they should be placed
in the 3D space. The file that describes that is called the world file,
which also follows the SDF format and it includes other SDF files. For example
if you have an SDF file that describes your robot and another SFF file that
describes a ball that the robot needs to pick up, you would also need to
create a world file that describes where in the space the robot starts
and where the ball is initially placed. Once the Gazebo reads in the
world file, it will place all objects in it and start simulating the physical
interaction. For example, if the simulation process (explained later) results
in applying a torque on the wheel axle, the robot will move.

Whether it will
move correctly towards the ball depends on whether your program is doing the
right thing, but that is a topic that we will touch later. At this time
you need to understand that for a successful simulation, you will need
SDF files, some of which you will have to write from scratch and the world
file. You will not need to write the SDF file for everything. There are many
pre-existing models that are publicly available (including the FRC field
and game elements from previous FRC seasons). You can use these models
in your world to build the habitat for your quickly.

## Plug-ins

Very often, physics simulation alone is not enough to capture the
true behavior of the robot. Specifically, Gazebo simulation starts from
forces and torques being applied to joints. From there, Gazebo uses
the information found in the SDF file and the world file to compose a set
of differential equations that it needs to solve. It uses known laws
of physics built into the Gazebo code to construct the problem and solve
it. The solution is done numerically: Starting from the current positions
of all links and velocities of all joints, Gazebo takes the forces and torques
and calculates what will happen after some tiny time increment. Then it repeats
the process.

All of the above sounds great until we realize that the robot code does not
directly produce the torque on an axle. What it does instead is that it
sends data to the motor controller, which in turns generates the voltage
on the motor (typically using a PWM signal) and the torque comes out as
an indirect result of the applied voltage and the motor dynamics (i.e.
electro-mechanical equations that govern the behavior of the motor). Gazebo
does not model electrical components of the robot, such as motors. To
capture that behavior you need to write a small C++ program and give it
to Gazebo to use it. This is called a plug-in. Any program that Gazebo
executes in addition to solving the physics of the world is called a plug-in.

Plugins are even more important when simulating sensors. For example if you
are using encoders in your robot, Gazebo will only give you the position
of the link associated with the wheel axle. Encoder on the other hand
gives to your software a number that you need to process in order to
deduce how fast your wheel is spinning or what the position of your actuator
is. To model the behavior of an encoder you need a plugin that will
read the joint position or velocity from Gazebo state and present it
to your program in the format that matches what the real encoder would do.
More advanced the sensor is, more complex the plug-in code is.

So, in your modeling activity, you will be writing or at least modifying the
plugins. The FRC WPILib software provides a few plugins that can capture vast
majority of behaviors we will need, so we will not need to jump to writing
plugins right away, but as our models get more advanced we will get to that
too.

## Topics

Now that you understand how Gazebo simulates the physics you are probably
asking yourself how does it communicate with your program. On a real robot,
your program executes on the
[RoboRIO](https://docs.wpilib.org/en/stable/docs/software/roborio-info/index.html) unit. Your program via calls to WPILib eventually pokes the data into
motor controllers or reads the data from sensor interfaces which interface
with the actual robot through the circuits that you build. Think of the
WPILib together with all circuits on your robot as a communication
channel between your program and the robot's actuators and sensors.

In simulation this communication channel must be simulated, which in
Gazebo is implemented through so called topics. Topics are network
connections that connect two programs. The two programs can run
on the same or different computer.
As we have learned so far, actuators and sensors are simulated
in Gazebo as a combination of SDF files and plug-ins. So Gazebo together
with your models replaces the real robot and the real world around it.

Your program in simulation runs on your local computer instead of the
RoboRIO unit. It links with the same WPILib library that is used on
real robot except that the part that deals with the hardware is
replaced with the part that uses Gazebo topics to communicate with Gazebo.
This part of the WPILib library is called HAL, which stands for Hardware
Abstraction Layer. On the Gazebo side, your model plug-in also taps into
the topics and communicates with the HAL.

To make this clear, let's go through an example. Consider the case when your
program sets the speed on a motor (this is the simplest example that you
have probably already done many times on the real robot). Your program
makes a call into WPILib function, the information propagates down to lower
layers of the library until it reaches the HAL. This is where things start
to differ from a real robot. Instead of dealing with the nitty-gritty
of the hardware, HAL redirects this the motor speed information into a Gazebo
topic. The information flows (over a network connection) to the Gazebo
plug-in that runs as part of your robot model. The plugin then simulates
the electrical behavior of the motor and calculates the torque to apply
on the axle associated with the motor. From there, Gazebo's physics solver
takes over and calculates the motion. The time is advanced and the
process repeats.

When preparing your simulation models, you will not need to know exact details
of how the above mechanism is implemented but you will need to understand
the general process. You will need to know on which topic a specific
motor controller in your program will send information and how to specify in
the SDF file which topic in a plugin will affect which joint. This specification
in the SDF file effectively simulates the wiring of your robot.

## Gazebo and ROS

As you read the documentation and forums about Gazebo (which you should
do), you will often encounter the mention of ROS (Robot Operating System).
This is because Gazebo is often used as a simulator for ROS-based robots.

ROS is a very powerful framework for writing robot software. ROS does the
similar thing in its own framework to what WPILib does in FRC. So
WPILib-based code is not meant
to be used with ROS. This can be done with some additional programming which
is outside the scope of this tutorial. In this tutorial we assume that we
are not using ROS.

While Gazebo is often used with ROS (and both tools are developed
and maintained by the same organization), this is not a requirement.
Gazebo is a stand-alone and independent simulator that does not require
ROS. Unless your FRC code uses ROS for some other reason, we will not be
using it as a Gazebo component. If you read any text about "ROS topics", do
not be confused. ROS has its own communication system that is also called
"topics". Also some Gazebo plugins are meant for use with ROS. These won't
work in our non-ROS simulation of the FRC code.

# Simulator Setup

Now that you understand how the simulation works, it is time to try it
out in practice. We will walk through the one-time setup and regular
usage.

## General WPILib and VS Code setup

First, you should make sure that you have a working environment for FRC
code development, which includes VSCode, WPILib, and WPILib extensions
for VSCode. If you have done development for FRC you probably already
have everything set up. Note that to use Gazebo, you will need a Linux
machine (instructions in this tutorial are based on Ubuntu 18.04, although
other distributions should work as well with some differences). So if your
development environment was based on Windows, you should repeat the
[setup for a Linux machine](https://docs.wpilib.org/en/latest/docs/zero-to-robot/step-2/wpilib-setup.html). After the installation verify that the
tools are working by creating a simple application and compiling it, you
should also try to run the WPILib integrated simulator by following
the [WPILib simulation instructions](https://docs.wpilib.org/en/latest/docs/software/wpilib-tools/robot-simulation/index.html)

## Gazebo Installation

Next, you must install Gazebo. This tutorial is based on Gazebo 9, so this
should be your preferred version. Newer versions of Gazebo should work
too, but your friendly author has not verified it. If you happen to be
a ROS user (outside the scope of this tutorial) and have ROS already
installed, then you probably have Gazebo 9 installed as well, so you do
not need to do anything. If you are installing Gazebo on a Linux
machine without ROS, then follow the instructions available on
[Gazebo web site](http://gazebosim.org/).

To verify that everything is working, you can start Gazebo by typing
`gazebo` in your shell. The Gazebo window will open and show you an
empty world. Also verify that the public models library is available
(you must be connected to Internet). Click on the "Insert" tab in the left
section of the window and look for the `http://gazebosim.org/models` section.
Click on it to expand it and you will see a list of models. You can click on
any model and drop it in the world. If you scroll down you will find a few
FRC models including the famous trash can from 2015 game.

## Directory for your FRC Work

To make running the simulation easier you should organize your directories
in the way that is consistent with the assumptions made by this tutorial.
While everything can be made to work regardless of where you
place the files, if you follow the same directory structure as described
here, then all commands and scripts that we later refer to will just work.
If you follow your own convention, then you are on your own in
adjusting the scripts and environment variables.

The first convention that we will establish is that we will keep all
files under a directory that is pointed by $FRC_ROOT environment variable.
So first create the directory (it can be right under the top of your
home directory) for all your FRC stuff. Type this:

```
cd ~
mkdir FRC
```

Next use your favorite editor, open the file `.bashrc` that resides
under the top of your home directory. Add this line to it:

```
export FRC_ROOT=~/FRC
export PATH=$PATH:$FRC_ROOT/frc_gazebo_2021/scripts
```

Now close the shell and open the new one and type:

```
echo $FRC_ROOT
```

If you get nothing in response, then you made a mistake somewhere. If it
returns the full path to your `FRC` directory, then it's all good.
All scripts used by this tutorial will refer to this environment variable,
so make sure it is set correctly. If you move your `FRC` directory somewhere
else, you will have to change the value of the variable.

Next, clone the repository that contains this tutorial. Type this:

```
git clone https://github.com/ihadzic/frc_gazebo_2021.git $FRC_ROOT/frc_gazebo_2021
```

You should see the new directory called `frc_gazebo_2021` under your `FRC`
directory. Up until this point, you have probably been reading these
instructions directly from Github. Now that you have cloned the tutorial
repository you can find this exact file under
`$FRC_ROOT/frc_gazebo_2021/doc/tutorial.md`.

## One-time setup

At this time you should have the directory for your FRC work set up
and your `$FRC_ROOT` environment variable should point to it. Under your
`$FRC_ROOT` directory, there should be the `frc_gazebo_2021` directory
which is a clone of this repository. Please verify that everything
is correct before proceeding because we are about to run a script that
will populate your FRC directory with the rest of the stuff that you will
need. If you have not set things up correctly so far, script may end up
putting stuff where you don't want it. If you are ready, proceed. Type:

```
frc_gaz_init
```

If you would like to understand how the script works, you can find it under
`$FRC_ROOT/frc_gazebo_2021/scripts`. We will briefly explain what it does,
so you don't have to look into the script right away, but for your own
learning you should eventually do it.
