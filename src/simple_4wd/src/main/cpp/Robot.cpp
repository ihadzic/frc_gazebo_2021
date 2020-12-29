/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/Joystick.h>
#include <frc/PWMVictorSPX.h>
#include <frc/TimedRobot.h>
#include <frc/Timer.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/livewindow/LiveWindow.h>

class Robot : public frc::TimedRobot {
 public:
  Robot() {
    m_robotDrive_front.SetExpiration(1.0);
    m_robotDrive_back.SetExpiration(1.0);
    m_timer.Start();
  }

  void AutonomousInit() override {
    m_timer.Reset();
    m_timer.Start();
  }

  void AutonomousPeriodic() override {
    // Drive for 2 seconds
    if (m_timer.Get() < 20.0) {
      // Drive forwards half speed
      m_robotDrive_front.ArcadeDrive(-0.5, 0.0);
      m_robotDrive_back.ArcadeDrive(-0.5, 0.0);
    } else {
      // Stop robot
      m_robotDrive_front.ArcadeDrive(0.0, 0.0);
      m_robotDrive_back.ArcadeDrive(0.0, 0.0);
    }
  }

  void TeleopInit() override {}

  void TeleopPeriodic() override {
    // Drive with arcade style (use right stick)
    double joyY = m_stick.GetY();
    double joyX = m_stick.GetX();
    m_robotDrive_front.ArcadeDrive(joyY, joyX);
    m_robotDrive_back.ArcadeDrive(joyY, joyX);
  }

  void TestPeriodic() override {}

 private:
  // Robot drive system
  frc::PWMVictorSPX m_left_front{1};
  frc::PWMVictorSPX m_right_front{3};
  frc::PWMVictorSPX m_left_back{2};
  frc::PWMVictorSPX m_right_back{4};
  frc::DifferentialDrive m_robotDrive_front{m_left_front, m_right_front};
  frc::DifferentialDrive m_robotDrive_back{m_left_back, m_right_back};

  frc::Joystick m_stick{0};
  frc::LiveWindow& m_lw = *frc::LiveWindow::GetInstance();
  frc::Timer m_timer;
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
