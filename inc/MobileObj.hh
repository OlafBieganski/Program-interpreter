#ifndef MOBILE_OBJ_HH
#define MOBILE_OBJ_HH

#include "AbstractMobileObj.hh"
#include <string>
#include <mutex>

class MobileObj : public AbstractMobileObj 
{
    std::string name;
    unsigned int _color[3];
    Vector3D _scale;
    Vector3D _shift;
    Vector3D _position;
    double _roll_deg;
    double _pitch_deg;
    double _yaw_deg;
    std::mutex mtx;

    public:
    MobileObj(const AbstractMobileObj& other) {};
    double GetAng_Roll_deg() const { return _roll_deg; }
    double GetAng_Pitch_deg() const { return _pitch_deg; }
    double GetAng_Yaw_deg() const { return _yaw_deg; }
    void SetAng_Roll_deg(double Ang_Roll_deg) { _roll_deg = Ang_Roll_deg; }
    void SetAng_Pitch_deg(double Ang_Pitch_deg) { _pitch_deg = Ang_Pitch_deg; }
    void SetAng_Yaw_deg(double Ang_Yaw_deg) { _yaw_deg = Ang_Yaw_deg; }
    const Vector3D & GetPosition_m() const { return _position; }
    //Vector3D & UsePosition_m() { return _position; }
    void SetPosition_m(const Vector3D &rPos_m) { _position = rPos_m; }
    void SetName(const char* sName) { name = sName; }
    const std::string & GetName() const { return name; }
    void LockAccess() { mtx.lock(); }
    void UnlockAccess() { mtx.unlock(); }
};

#endif