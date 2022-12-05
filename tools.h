//
// Created by Jeroen Vandezande on 05/12/2022.
//

#ifndef _TOOLS_H_
#define _TOOLS_H_

namespace LowLevelEmbedded
{
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
}
#endif //_TOOLS_H_
