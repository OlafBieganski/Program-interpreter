#include <iostream>
#include "Interp4Rotate.hh"
#include "MobileObj.hh"
#include <thread>
#include <sstream>


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Rotate"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Rotate::CreateCmd();
}


/*!
 *
 */
Interp4Rotate::Interp4Rotate()
{}


/*!
 *
 */
void Interp4Rotate::PrintCmd() const
{
	cout << GetCmdName() << " " << obj_name << " " 
		<< axis_name << " " << ang_vel << " " <<
		rot_angle << endl;
}


/*!
 *
 */
const char* Interp4Rotate::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Rotate::ExecCmd( AbstractScene &rScn, AbstractComChannel &rComChann)
{
    this->PrintCmd();

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
    
	Vector3D rotation, curr_rot, result_rot;
	unsigned int idx;
	if(axis_name == "OX")
	{
		idx = 0;
		rotation[0] = rot_angle;
	}
	else if(axis_name == "OY")
	{
		idx = 1;
		rotation[1] = rot_angle;
	}
	else if(axis_name == "OZ")
	{
		idx = 2;
		rotation[2] = rot_angle;
	}
	else
	{
		std::cerr << "Unrecognized parameter in Rotate command.\n";
		return false;
	}

	obj->LockAccess();
	curr_rot[0] = obj->GetAng_Roll_deg();
    curr_rot[1] = obj->GetAng_Pitch_deg();
    curr_rot[2] = obj->GetAng_Yaw_deg();
	obj->UnlockAccess();

    result_rot = curr_rot; // najpierw zaczynamy od poczatkowej orinetacji dla kazdej skladowej

    for (int i = 0; i < 100; ++i)
    {
		// inkrementujemy skladaowa wokol ktorej rotujemy
        result_rot[idx] = curr_rot[idx] + (rotation[idx] / 100.0) * (double)(i);
        std::stringstream ss;
        ss << "UpdateObj " << "Name=" << obj_name << " RotXYZ_deg=" << result_rot << "\n";
		rComChann.LockAccess();
		rComChann.Send(ss.str().c_str());
		rComChann.UnlockAccess();
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 / ang_vel)));
    }

	obj->LockAccess();
	obj->SetAng_Roll_deg(result_rot[0]);
	obj->SetAng_Pitch_deg(result_rot[1]);
	obj->SetAng_Yaw_deg(result_rot[2]);
	obj->UnlockAccess();

    return true;
}


/*!
 *
 */
bool Interp4Rotate::ReadParams(std::istream& Strm_CmdsList)
{
	if(Strm_CmdsList.good())
  {
    Strm_CmdsList >> obj_name >> axis_name >> ang_vel
      >> rot_angle;
    return true;
  }
	return false;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Rotate::CreateCmd()
{
  return new Interp4Rotate();
}


/*!
 *
 */
void Interp4Rotate::PrintSyntax() const
{
  cout << "Rotate nazwa_obiektu nazwa_osi szybkosc_katowa kat_obrotu" << endl;
}
