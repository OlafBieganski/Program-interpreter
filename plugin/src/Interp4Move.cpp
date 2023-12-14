#include <iostream>
#include "Interp4Move.hh"
#include "Vector3D.hh"
#include "MobileObj.hh"
#include "ComChannel.hh"
#include <thread>
#include <chrono>
#include <sstream>

using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Move"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Move::CreateCmd();
}


/*!
 *
 */
Interp4Move::Interp4Move()
{}


/*!
 *
 */
void Interp4Move::PrintCmd() const
{
	cout << GetCmdName() << " " << obj_name << " " 
		<< speed << " " << distance << endl;
}


/*!
 *
 */
const char* Interp4Move::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Move::ExecCmd( AbstractScene &rScn, AbstractComChannel &rComChann)
{
  	PrintCmd();

    AbstractMobileObj* obj;
    try
    {
        obj = rScn.FindMobileObj(obj_name.c_str());
    }
    catch (std::out_of_range &error)
    {
        std::cerr << "Failed to find object: " << obj_name << " in Interp4Move::ExecCmd! Command had no effect." << std::endl;

        return false;
    }

	obj->LockAccess();
    auto currPose = obj->GetPosition_m();
    double rot_vec[3] = {obj->GetAng_Roll_deg() * 3.14 / 180, obj->GetAng_Pitch_deg() * 3.14 / 180, obj->GetAng_Yaw_deg() * 3.14 / 180};
	obj->UnlockAccess();

	Vector3D trans_vec;
    trans_vec[0] = (cos(rot_vec[0]) * sin(rot_vec[1]) * cos(rot_vec[2]) + sin(rot_vec[0]) * sin(rot_vec[2])) * distance;
    trans_vec[1] = (cos(rot_vec[0]) * sin(rot_vec[1]) * sin(rot_vec[2]) - sin(rot_vec[0]) * cos(rot_vec[2])) * distance;
    trans_vec[2] = cos(rot_vec[0]) * cos(rot_vec[1]) * distance;
	
    Vector3D newPose;
    for (int i = 0; i < 100; ++i)
    {
        newPose[0] = currPose[0] + (trans_vec[0] / 100.0) * (double)(i);
        newPose[1] = currPose[1] + (trans_vec[1] / 100.0) * (double)(i);
        newPose[2] = currPose[2] + (trans_vec[2] / 100.0) * (double)(i);
        std::stringstream ss;
        ss << "UpdateObj "
           << "Name=" << obj_name << " Trans_m=" << newPose << "\n";
		//std::cout << ss.str();
		rComChann.LockAccess();
        rComChann.Send(ss.str().c_str());
        rComChann.UnlockAccess();
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 / speed)));
    }
	obj->LockAccess();
    obj->SetPosition_m(newPose);
	obj->UnlockAccess();
    return true;
}


/*!
 *
 */
bool Interp4Move::ReadParams(std::istream& Strm_CmdsList)
{
	if(Strm_CmdsList.good())
	{
    Strm_CmdsList >> obj_name >> speed >> distance;
    return true;
  }
  return false;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Move::CreateCmd()
{
  return new Interp4Move();
}


/*!
 *
 */
void Interp4Move::PrintSyntax() const
{
  cout << "   Move  NazwaObiektu  Szybkosc[m/s]  DlugoscDrogi[m]" << endl;
}
