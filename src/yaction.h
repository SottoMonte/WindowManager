#ifndef __YACTION_H
#define __YACTION_H

/** tActionId describes an internal identifier for action triggers within icewm.
 * See wmaction.h for detailed explanation and assignment rules.
 *
 * It's not an int because some code uses object pointers as local action ID
 * which is passed back through callbacks, therefore the pointer size must be
 * guaranteed.
 */
typedef size_t tActionId;
/** Create a unique id for actions which are not handled by global EWmAction or
 * other static lists (enumerations). */
tActionId genActionId();

class YActionListener {
public:
    virtual void actionPerformed(tActionId action, unsigned int modifiers) = 0;
protected:
    virtual ~YActionListener() {};
};

#endif

// vim: set sw=4 ts=4 et:
