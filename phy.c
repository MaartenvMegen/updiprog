#include <unistd.h>
#include "com.h"
#include "log.h"
#include "phy.h"
#include "updi.h"
#include "sleep.h"

uint32_t baudrate_setting  =9600;
uint8_t retries = 0;

/** \brief Initialize physical interface
 *
 * \param [in] port Port name as string
 * \param [in] baudrate Transmission baudrate
 * \param [in] onDTR True if using DTR for power
 * \return true if success
 *
 */
bool PHY_Init(char *port, uint32_t baudrate, bool onDTR)
{
  baudrate_setting=baudrate;
  retries=0;
  LOG_Print(LOG_LEVEL_INFO, "Initializing phy %s at %u baud\n", port, baudrate);
  return COM_Open(port, baudrate, true, true);
}

/** \brief Sends a double break to reset the UPDI port
 *         BREAK is actually just a slower zero frame
 *         A double break is guaranteed to push the UPDI state
 *         machine into a known state, albeit rather brutally
 *
 * \param [in] port Port name as string
 * \return true if success
 *
 */
bool PHY_DoBreak(char *port, uint32_t baudrate)
{
  uint8_t buf[] = {UPDI_BREAK, UPDI_BREAK};
  LOG_Print(LOG_LEVEL_INFO, "Sending double break @ %u", baudrate);


  COM_Close();
  if (COM_Open(port, baudrate, false, false) != true)
    return false;
  // Send two break characters, with 1 stop bit in between
  COM_Write(buf, sizeof(buf));

  // Wait for the double break end
  msleep(10);  // wait for 10mS second
  if (COM_Read(buf, 2) != 2)
    LOG_Print(LOG_LEVEL_WARNING, "No answer received");

  COM_Close();

  // Re open port at original baudrate setting.
  COM_Open(port, baudrate_setting, false, false);

  return true;
}

/** \brief Send data to physical interface
 *
 * \param [in] data Buffer with data
 * \param [in] len Length of data buffer
 * \return true if success
 *
 */
bool PHY_Send(uint8_t *data, uint8_t len)
{
  //uint8_t i;

  /*for (i = 0; i < len; i++)
  {
    COM_Write(&data[i], 1);
  }*/
  COM_Write(data, len);
  // read echo
  //usleep(10);
  //msleep(COM_GetTransTime(len));
  //Sleep(10);

  COM_Read(data, len);

  return true;
}

/** \brief Receive data from physical interface to data buffer
 *
 * \param [out] data Data buffer to write data in
 * \param [in] len Length of data to be received
 * \return true if success
 *
 */
bool PHY_Receive(uint8_t *data, uint16_t len)
{
  int val = COM_Read(data, len);
  if ((val < 0) || (val != len))
    return false;
  return true;
}

/** \brief Close physical interface
 *
 * \return Nothing
 *
 */
void PHY_Close(void)
{
  COM_Close();
}
