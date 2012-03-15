//////////////////////////////////////////////////////////////////////
//
// multiplaymgr.hpp
//
// Written by Duncan McCreanor, started February 2003.
// duncan.mccreanor@airservicesaustralia.com
//
// Copyright (C) 2003  Airservices Australia
// Copyright (C) 2005  Oliver Schroeder
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// $Id$
//  
//////////////////////////////////////////////////////////////////////

#ifndef MULTIPLAYMGR_H
#define MULTIPLAYMGR_H

#define MULTIPLAYTXMGR_HID "$Id$"

#include "mpmessages.hxx"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;

#include <simgear/compiler.h>
#include <simgear/props/props.hxx>
//#include <plib/netSocket.h>
//#include <Main/globals.hxx>

//#include <AIModel/AIMultiplayer.hxx>

struct FGExternalMotionInfo;

// interfaces to make code wrapping easier
class FGAIMultiplayer {
public:
    long getLastTimestamp() { return m_stamp; }
    void setDie(bool die) { m_die = die;}
    void setPath(string path) { m_path = path; }
    void setCallSign(string callSign) { m_callSign = callSign; }
    void addMotionInfo(FGExternalMotionData & motionInfo, long & stamp) { m_motionInfo = motionInfo; m_stamp = stamp; }
private:
    string m_callSign;
    string m_path;
    FGExternalMotionData m_motionInfo;
    long m_stamp;
    bool m_die;
};

int fgGetInt(string name);
string fgGetString(string name);

class FGMultiplayMgr 
{
public:

  struct IdPropertyList {
    unsigned id;
    const char* name;
    SGPropertyNode::Type type;
  };
  static const IdPropertyList sIdPropertyList[];
  static const unsigned numProperties;

  static const IdPropertyList* findProperty(unsigned id);
  
  FGMultiplayMgr();
  ~FGMultiplayMgr();
  bool init (const short rxPort, const string rxAddress, const short txPort, const string txAddress, const string callsign ) ;
  void Close(void);
  // transmitter
  void SendMyPosition(const FGExternalMotionData& motionInfo);
  void SendTextMessage(const string &sMsgText);
  void FillMsgHdr(T_MsgHdr *MsgHdr, int iMsgId, unsigned _len = 0u);
  
  // receiver
  void ProcessPosMsg(const char *Msg, netAddress & SenderAddress,
                     unsigned len, long stamp);
  void ProcessChatMsg(const char *Msg, netAddress & SenderAddress);
  void Update(void);
  
private:
  FGAIMultiplayer* addMultiplayer(const std::string& callsign,
                                  const std::string& modelName);
  FGAIMultiplayer* getMultiplayer(const std::string& callsign);

  /// maps from the callsign string to the FGAIMultiplayer
  typedef std::map<std::string, FGAIMultiplayer * > MultiPlayerMap;
  MultiPlayerMap mMultiPlayerMap;

  netSocket* mSocket;
  netAddress mServer;
  bool mHaveServer;
  bool mInitialised;
  string mCallsign;
};

#endif

