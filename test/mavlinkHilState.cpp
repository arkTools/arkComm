#include "AsyncSerial.hpp"
#include <iostream>
#include <stdexcept>
#include <boost/timer.hpp>
#include <inttypes.h>

// mavlink system definition and headers
#include "mavlink/v1.0/common/mavlink.h"


class MavlinkHilState {

private:

    // private attributes
    
    mavlink_system_t _system;
    mavlink_status_t _status;
    boost::timer _clock;
    BufferedAsyncSerial * _comm;
    static const double _rad2deg = 180.0/3.14159;

    // private methods
    
    // send a mavlink message to the comm port
    void _sendMessage(const mavlink_message_t & msg) {
        uint8_t buf[MAVLINK_MAX_PACKET_LEN];
        uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
        _comm->write((const char *)buf, len);
    }

public:
    MavlinkHilState(const uint8_t sysid, const uint8_t compid, const MAV_TYPE type,
            const std::string & device, const uint32_t baudRate) : 
        _system(), _status(), _clock(), _comm() {
          
        // system
        _system.sysid = sysid;
        _system.compid = compid;
        _system.type = type;

        // start comm
        try
        {
            _comm = new BufferedAsyncSerial(device,baudRate);
        }
        catch(const boost::system::system_error & e)
        {
            std::cout << "error: " << e.what() << std::endl;
            exit(1);
        }
    }

    ~MavlinkHilState() {
        if (_comm)
        {
            delete _comm;
            _comm = NULL;
        }
    }
    
    void send() {
        // attitude states (rad)
        float roll = 1;
        float pitch = 2;
        float yaw = 3;

        // body rates
        float rollRate = 0.1;
        float pitchRate = 0.2;
        float yawRate = 0.3;

        // position
        int32_t lat = 1*_rad2deg*1e7;
        int32_t lon = 2*_rad2deg*1e7;
        int16_t alt = 3*1e3;

        int16_t vx = 1*1e2;
        int16_t vy = 2*1e2;
        int16_t vz = -3*1e2;

        int16_t xacc = 1*1e3;
        int16_t yacc = 2*1e3;
        int16_t zacc = 3*1e3;

        mavlink_message_t msg;
        mavlink_msg_hil_state_pack(_system.sysid, _system.compid, &msg, 
            _clock.elapsed(),
            roll,pitch,yaw,
            rollRate,pitchRate,yawRate,
            lat,lon,alt,
            vx,vy,vz,
            xacc,yacc,zacc);
        _sendMessage(msg);
    }

    void receive() {

        // receive messages
        mavlink_message_t msg;
        while(_comm->available())
        {
            uint8_t c = 0;
            if (!_comm->read((char*)&c,1)) return;

            // try to get new message
            if(mavlink_parse_char(MAVLINK_COMM_0,c,&msg,&_status))
            {
                switch(msg.msgid)
                {

                case MAVLINK_MSG_ID_HIL_CONTROLS:
                {
                    //std::cout << "receiving messages" << std::endl;
                    mavlink_hil_controls_t hil_controls;
                    mavlink_msg_hil_controls_decode(&msg,&hil_controls);
                    std::cout << "roll: " << hil_controls.roll_ailerons << std::endl;
                    std::cout << "pitch: " << hil_controls.pitch_elevator << std::endl;
                    std::cout << "yaw: " << hil_controls.yaw_rudder << std::endl;
                    std::cout << "throttle: " << hil_controls.throttle << std::endl;
                    std::cout << "mode: " << hil_controls.mode << std::endl;
                    std::cout << "nav mode: " << hil_controls.nav_mode << std::endl;
                    break;
                }

                default:
                {
                    std::cout << "received message: " << uint32_t(msg.msgid) << std::endl;
                }

                }
            }
        }
    }
};

int main (int argc, char const* argv[])
{
    if (argc != 3) {
        std::cout << "usage: mavlinkState device baud" << std::endl;
        return 1;
    };
    std::string device(argv[1]);
    uint32_t baud = atoi(argv[2]);

    std::cout << "device: " << device << std::endl;
    std::cout << "baud: " << baud << std::endl;
    MavlinkHilState test(0,0,MAV_TYPE_GENERIC,device,baud); 

    while(1) {
        test.send();
        test.receive();
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }
    return 0;
}

// vim:ts=4:sw=4:expandtab
