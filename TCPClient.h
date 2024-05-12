/**
 ***************************************************************************
 * Description       : Header for TCP command handler
 * Compiler          : C++
 *
 * Copyright (C) 2017, Peter Kwan
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaims all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 ***************************************************************************
 * Handle network control commands.
 * Initially for Newfor subtitles but can also be used for other inserter
 * control functions.
 * These inserter functions follow MRG Systems' XTP620 protocol
 *
 * Example:
 * To find out what model of inserter you are connected to, type Y.
 * You will need to enable Telnet (Windows) or run nc (Linux).
 * Telnet can be enabled in the "Turn Windows features on or off" settings.
 * Connect to the IP address and port of your VBIT "telnet 192.168.1.2 5570"
 * Type Y<enter>
 * Should reply "VBIT620"
 * Use CTRL-] to exit telnet
**/

#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#endif

#include <stdint.h>

#include "pagelist.h"
#include "newfor.h"

#include "debug.h"
#include "packetsubtitle.h"

namespace vbit
{
    class TCPClient
    {
        public:
            TCPClient(Debug* debug, PacketSubtitle* subtitle, ttx::PageList* pageList);
            ~TCPClient();
            void Handler(int clntSocket);

        private:
            Debug* _debug;
            
            // Constants
            static const uint8_t MAXCMD=128;
            static const uint8_t RCVBUFSIZE=132;   /* Size of receive buffer */

            // Normal command mode
            static const uint8_t MODENORMAL=0;
            static const uint8_t MODESOFTELPAGEINIT=1;
            // Get row count
            static const uint8_t MODEGETROWCOUNT=3;
            // Get a row of data
            static const uint8_t MODEGETROW=4;
            // Display the row
            static const uint8_t MODESUBTITLEONAIR=5;
            // Clear down
            static const uint8_t MODESUBTITLEOFFAIR=6;
            static const uint8_t MODESUBTITLEDATAHIGHNYBBLE=7;
            static const uint8_t MODESUBTITLEDATALOWNYBBLE=8;

            // Variables
            char _cmd[MAXCMD];  // command buffer
            char* _pCmd;        // Pointer into the command buffer
            Newfor _newfor;
            int _row;           // Row counter
            char* _pkt;         // A teletext packet (one row of VBI)
            int _rowAddress;    // The address of this row

            /** The page address in MPPSS (hex. 10000..8FF99. Omitting trailing characters defaults to 0
             * wildcard
             */
            char _pageNumber[6];

            ttx::PageList* _pageList; // List of pages for XTP620 commands to access


            // Functions
            void clearCmd(void);
            void addChar(char ch, char* response);
            void command(char* cmd, char* response); // Handles XPT620 commands
            void DieWithError(std::string errorMessage);

            /** Validate a page identity of the form mppss
             *  Where:
             *  m=1..8
             *  p=0..f
             *  s=0,,9
             *  and any digit can be wildcarded with *
             *  and trailing digits can be omitted where they will be defaulted to 0
             * \param src - the page identity parameter of a P command. (only one allowed in this implementation)
             * \param dest - the validated page number
             * \return true if the page identity is valid
             */
            bool Validate(char* dest, char* src);
    };
}
#endif
