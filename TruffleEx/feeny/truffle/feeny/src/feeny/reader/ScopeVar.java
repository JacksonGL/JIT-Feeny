package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

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
        throw new UnsupportedOperationException("ScopeVar is not supported yet");
    }
}
