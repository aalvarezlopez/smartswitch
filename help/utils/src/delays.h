/*
 * delays.h
 *
 *  Created on: Dec 2, 2013
 *      Author: Adrian
 */

#ifndef DELAYS_H_
#define DELAYS_H_


#define CLOCK_HZ    48000000

/**
 * \brief Delay loop to delay n number of cycles
 *
 * \note The function runs in internal RAM so that flash wait states
 *       will not affect the delay time.
 *
 * \param n Number of cycles
 */

#define cpu_ms_2_cy(ms, f_cpu)  \
    (((uint64_t)(ms) * (f_cpu) + (uint64_t)(14e3-1ul)) / (uint64_t)14e3)
#define cpu_us_2_cy(us, f_cpu)  \
    (((uint64_t)(us) * (f_cpu) + (uint64_t)(14e6-1ul)) / (uint64_t)14e6)

#define delay_cycles               portable_delay_cycles

#define cpu_delay_ms(delay, f_cpu) delay_cycles(cpu_ms_2_cy(delay, CLOCK_HZ))
#define cpu_delay_us(delay, f_cpu) delay_cycles(cpu_us_2_cy(delay, CLOCK_HZ))
//! @}

/**
 *\def delay_ms
 *\brief Delay in milliseconds.
 *\param delay Delay in milliseconds
 */
#define delay_ms(delay)     cpu_delay_ms(delay, F_CPU)

/**
 * @def delay_us
 * @brief Delay in microseconds.
 * @param delay Delay in microseconds
 */
#define delay_us(delay)     cpu_delay_us(delay, F_CPU)

void portable_delay_cycles(unsigned long n);

#endif /* DELAYS_H_ */
