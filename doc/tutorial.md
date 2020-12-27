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
Originally, Gazebo support (through FRCSim project) for FRC slowed down in
year 2016 and completely stopped afterwards to be replaced with Synthesis.
However, based on few recent commits in WPILib source code, it seems like
there is an intent to revive Gazebo support.

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
