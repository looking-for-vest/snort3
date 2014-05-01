/*
** Copyright (C) 2014 Cisco and/or its affiliates. All rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

// ppm_module.cc author Russ Combs <rucombs@cisco.com>

#include "ppm_module.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ppm.h"
#include "main/snort_config.h"

//-------------------------------------------------------------------------
// ppm attributes
//-------------------------------------------------------------------------

#ifdef PPM_MGR
static const Parameter ppm_params[] =
{
    { "max_pkt_time", Parameter::PT_INT, "0:", "0",
      "enable packet latency thresholding (usec), 0 = off" },

    { "fastpath_expensive_packets", Parameter::PT_BOOL, nullptr, "false",
      "stop inspection if the max_pkt_time is exceeded" },

    { "pkt_log", Parameter::PT_ENUM, "none | log | alert | both", "none",
      "log event if max_pkt_time is exceeded" },

#ifdef DEBUG
    { "debug_pkts", Parameter::PT_BOOL, nullptr, "false",
      "enable packet debug" },
#endif
    { "max_rule_time", Parameter::PT_INT, "0:", "0",
      "enable rule latency thresholding (usec), 0 = off" },

    { "threshold", Parameter::PT_INT, "1:", "5",
      "number of times to exceed limit before disabling rule" },

    { "suspend_expensive_rules", Parameter::PT_BOOL, nullptr, "false",
      "temporarily disable rule if threshold is reached" },

    { "suspend_timeout", Parameter::PT_INT, "0:", "60",
      "seconds to suspend rule, 0 = permanent" },

    { "rule_log", Parameter::PT_ENUM, "none|log|alert|both", "none",
      "enable event logging for suspended rules" },

#ifdef DEBUG
    { "debug_rules", Parameter::PT_BOOL, nullptr, "false",
      "enable rule debug" },
#endif
    { nullptr, Parameter::PT_MAX, nullptr, nullptr, nullptr }
};

#define PPM_EVENT_RULE_TREE_DISABLED_STR "(ppm) Rule Options Disabled by Rule Latency"
#define PPM_EVENT_RULE_TREE_ENABLED_STR  "(ppm) Rule Options Re-enabled by Rule Latency"
#define PPM_EVENT_PACKET_ABORTED_STR     "(ppm) Packet Aborted due to Latency"

static const RuleMap ppm_rules[] =
{
    { PPM_EVENT_RULE_TREE_DISABLED, PPM_EVENT_RULE_TREE_DISABLED_STR },
    { PPM_EVENT_RULE_TREE_ENABLED, PPM_EVENT_RULE_TREE_ENABLED_STR },
    { PPM_EVENT_PACKET_ABORTED, PPM_EVENT_PACKET_ABORTED_STR },

    { 0, nullptr }
};

//-------------------------------------------------------------------------
// ppm module
//-------------------------------------------------------------------------

PpmModule::PpmModule() : Module("ppm", ppm_params, ppm_rules) { }

bool PpmModule::set(const char*, Value& v, SnortConfig* sc)
{
    if ( v.is("max_pkt_time") )
        ppm_set_max_pkt_time(&sc->ppm_cfg, v.get_long());

    else if ( v.is("fastpath_expensive_packets") )
        ppm_set_pkt_action(&sc->ppm_cfg, PPM_ACTION_SUSPEND);

    else if ( v.is("pkt_log") )
    {
        unsigned u = v.get_long();
        if ( u & 0x1 )
            ppm_set_pkt_log(&sc->ppm_cfg, PPM_LOG_MESSAGE);
        if ( u & 0x2 )
            ppm_set_pkt_log(&sc->ppm_cfg, PPM_LOG_ALERT);
    }
#ifdef DEBUG
    else if ( v.is("debug_pkts") )
        ppm_set_debug_pkts(&sc->ppm_cfg, 1);
#endif
    else if ( v.is("max_rule_time") )
        ppm_set_max_rule_time(&sc->ppm_cfg, v.get_long());

    else if ( v.is("threshold") )
        ppm_set_rule_threshold(&sc->ppm_cfg, v.get_long());

    else if ( v.is("suspend_expensive_rules") )
        ppm_set_rule_action(&sc->ppm_cfg, PPM_ACTION_SUSPEND);

    else if ( v.is("suspend_timeout") )
        ppm_set_max_suspend_time(&sc->ppm_cfg, v.get_long());

    else if ( v.is("rule_log") )
    {
        unsigned u = v.get_long();
        if ( u & 0x1 )
            ppm_set_rule_log(&sc->ppm_cfg, PPM_LOG_MESSAGE);
        if ( u & 0x2 )
            ppm_set_rule_log(&sc->ppm_cfg, PPM_LOG_ALERT);
    }
#ifdef DEBUG
    else if ( v.is("debug_pkts") )
        ppm_set_debug_rules(&sc->ppm_cfg, 1);
#endif
    else
        return false;

    return true;
}
#endif

