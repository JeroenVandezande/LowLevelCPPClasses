//
// Created by Jeroen Vandezande on 05/12/2022.
//

#ifndef LOWLEVELCPP_TOOLS_H_
#define LOWLEVELCPP_TOOLS_H_

namespace LowLevelEmbedded
{
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
}
#endif //LOWLEVELCPP_TOOLS_H_
