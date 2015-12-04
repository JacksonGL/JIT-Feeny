package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.ScopeVarNode;

public class ScopeVar implements ScopeStmt {
    public final String name;
    public final Exp exp;

    public ScopeVar(String aName, Exp aExp) {
        name = aName;
        exp = aExp;
    }

    public String toString() {
        return "var " + name + " = " + exp;
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        fd.addFrameSlot(name, FrameSlotKind.Object);
        return new ScopeVarNode(name, exp.toTruffle(fd), fd);
    }
}
