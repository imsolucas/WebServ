/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etien <etien@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 11:44:54 by etien             #+#    #+#             */
/*   Updated: 2025/05/21 11:48:58 by etien            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <csignal>

// extern works like a forward declaration so that compiler references the variable
// and does not re-allocate it.
extern volatile sig_atomic_t gStopLoop;
