/**
 * @file vihcsr04.h
 * @author Ilia Voronin (www.linkedin.com/in/ilia-voronin-7a169122a)
 * @brief Header file of HC-SR04 ultrasonic distance sensor control driver
 *
 * @copyright Copyright (c) 2024 Ilia Voronin
 * 
 * This software is licensed under GNU GENERAL PUBLIC LICENSE 
 * The terms can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS,
 * Without warranty of any kind, express or implied, 
 * including but not limited to the warranties of merchantability, 
 * fitness for a particular purpose and noninfringement. 
 * In no event shall the authors or copyright holders be liable for any claim, 
 * damages or other liability, whether in an action of contract, tort or otherwise, 
 * arising from, out of or in connection with the software 
 * or the use or other dealings in the software.
 * 
 */

#ifndef VIHCSR04_H
#define VIHCSR04_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

bool VIHCSR04_Init();

#endif // VIHCSR04_H