//
// Created by Jeroen Vandezande on 05/12/2022.
//

#pragma once

namespace LowLevelEmbedded
{
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
}

