#include <iostream>
#include <fstream>
#include "Interp4Set.hh"
#include <thread>
#include <chrono>
#include <sstream>
#include "IOLock.hh"


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Set"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Set::CreateCmd();
}


/*!
 *
 */
Interp4Set::Interp4Set() {}


/*!
 *
 */
void Interp4Set::PrintCmd() const
{
	cout << GetCmdName() << " " << obj_name << " " 
		<< cord_x << " " << cord_y << " " <<
		cord_z <<" " << ang_ox << " " << ang_oy <<
		" " << ang_oz << endl;
}


/*!
 *
 */
const char* Interp4Set::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Set::ExecCmd( AbstractScene &rScn, AbstractComChannel &rComChann)
{
  	IOLock::io_lock();
    this->PrintCmd();
    IOLock::io_unlock();

    AbstractMobileObj* obj;
    try
    {
        obj = rScn.FindMobileObj(obj_name.c_str());
    }
    catch (std::out_of_range &error)
    {
        IOLock::io_lock();
        std::cerr << "Failed to find object: " << obj_name << " in Interp4Move::ExecCmd! Command had no effect." << std::endl;
        IOLock::io_unlock();

        return false;
    }
    
    obj->LockAccess();
    auto currPose = obj->GetPosition_m();
    obj->UnlockAccess();

    currPose[0] += cord_x;
    currPose[1] += cord_y;
    currPose[2] += cord_z;
    obj->LockAccess();
    obj->SetPosition_m(currPose);
    obj->UnlockAccess();
    std::stringstream ss;
    ss << "UpdateObj " << "Name=" << obj_name << " Trans_m=" << currPose << "\n";
    rComChann.LockAccess();
    rComChann.Send(ss.str().c_str());
    rComChann.UnlockAccess();
    return true;
}


/*!
 *
 */
bool Interp4Set::ReadParams(std::istream& Strm_CmdsList)
{
	if(Strm_CmdsList.good())
	{
		Strm_CmdsList >> obj_name >> cord_x >> cord_y
			>> cord_z >> ang_ox >> ang_oy >> ang_oz;
		return true;
	}
  	return false;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Set::CreateCmd()
{
  return new Interp4Set();
}


/*!
 *
 */
void Interp4Set::PrintSyntax() const
{
  cout << "Set nazwa_obiektu wsp_x wsp_y wsp_z kat_OX kat_OY kat_OZ" << endl;
}
