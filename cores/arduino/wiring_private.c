/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "wiring_private.h"

int pinPeripheral(uint32_t pin, EPioType peripheral)
{
  const PinDescription ppp = g_APinDescription[pin];

  // Handle the case the pin isn't usable as PIO
  if (ppp.ulPinType == PIO_NOT_A_PIN) {
    return -1;
  }

  // Disable peripheral muxing, done in pinMode
  //PORT->Group[ppp.ulPort].PINCFG[ppp.ulPin].bit.PMUXEN = 0;

  switch (peripheral)
  {
    case PIO_DIGITAL:
      // XXX: do we have to do something as all cases are covered?
      break;
    case PIO_INPUT:
      pinMode(pin, INPUT);
      break;
    case PIO_INPUT_PULLUP:
      pinMode(pin, INPUT_PULLUP);
      break;
    case PIO_OUTPUT:
      pinMode(pin, OUTPUT);
      break;

    case PIO_ANALOG:
    case PIO_SERCOM:
    case PIO_SERCOM_ALT:
    case PIO_TIMER:
    case PIO_TIMER_ALT:
    case PIO_EXTINT:
    case PIO_COM:
    case PIO_AC_CLK:
#if 0
      // Is the pio pin in the lower 16 ones?
      // The WRCONFIG register allows update of only 16 pin max out of 32
      if (ppp.ulPin < 16)
      {
        PORT->Group[ppp.ulPort].WRCONFIG.reg = PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_PMUX(peripheral) |
                                               PORT_WRCONFIG_WRPINCFG |
                                               PORT_WRCONFIG_PINMASK(ppp.ulPin);
      }
      else
      {
        PORT->Group[ppp.ulPort].WRCONFIG.reg = PORT_WRCONFIG_HWSEL |
                                               PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_PMUX(peripheral) |
                                               PORT_WRCONFIG_WRPINCFG |
                                               PORT_WRCONFIG_PINMASK(ppp.pin - 16);
      }
#else
      if (ppp.ulPin & 1) {
        // Odd pin

        // Get whole current setup for both odd and even pins and replace the odd one
        uint32_t temp = (PORT->Group[ppp.ulPort].PMUX[ppp.ulPin >> 1].reg) & PORT_PMUX_PMUXE(0xF);
        PORT->Group[ppp.ulPort].PMUX[ppp.ulPin >> 1].reg = temp | PORT_PMUX_PMUXO(peripheral);
      } else {
        // Even pin

        // Get whole current setup for both odd and even pins and replace the even one
        uint32_t temp = (PORT->Group[ppp.ulPort].PMUX[ppp.ulPin >> 1].reg) & PORT_PMUX_PMUXO(0xF);
        PORT->Group[ppp.ulPort].PMUX[ppp.ulPin >> 1].reg = temp | PORT_PMUX_PMUXE(peripheral);
      }

      // Enable port mux
      PORT->Group[ppp.ulPort].PINCFG[ppp.ulPin].reg |= PORT_PINCFG_PMUXEN; // Enable port mux
#endif
      break;

    case PIO_NOT_A_PIN:
      return -1l;
  }

  return 0l;
}

