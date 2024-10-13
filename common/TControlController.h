/** \file
    Small utility class to drive TControl Enabled state, disabling it in the scope
*/

#ifndef TControlControllerH
#define TControlControllerH

#include <assert.h>

class TControlController {
private:
    TControl *control;
public:
    TControlController(TControl *control): control(control) {
        assert(control);
        control->Enabled = false;
    }
    ~TControlController(void) {
        control->Enabled = true;
    }
};

#endif