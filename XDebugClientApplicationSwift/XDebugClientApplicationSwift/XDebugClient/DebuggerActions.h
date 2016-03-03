//
//  DebuggerActions.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/1/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef h_DebuggerActions
#define h_DebuggerActions

typedef enum {
    debugger_action_step_in,
    debugger_action_step_out,
    debugger_action_step_over,
    debugger_action_change_variables_depth,
    debugger_action_stop,
    debugger_action_run,
    
    debugger_variable_context_depth
} DebuggerActions;

#endif /* DebuggerActions_h */
