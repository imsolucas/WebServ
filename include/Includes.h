/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Includes.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imsolucas <imsolucas@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 17:12:28 by imsolucas         #+#    #+#             */
/*   Updated: 2025/05/27 17:48:39 by imsolucas        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_H
# define INCLUDES_H

// 🌐 C++ Standard Libraries
# include <iostream>
# include <exception>
# include <string>
# include <vector>
# include <iomanip>
# include <cstdlib>
# include <map>
# include <algorithm>
# include <fstream>

// 🧩 Project Headers
# include "Http.h"
# include "colors.h"
# include "utils.hpp"
# include "Exception.hpp"
# include "Config.hpp"
# include "signal.hpp"
# include "WebServer.hpp"
# include "Server.hpp"
# include "LocationConfig.hpp"

// 📄 Config Path
# define DEFAULT_CONFIG_PATH "config/default.conf"

#endif // INCLUDES_H
