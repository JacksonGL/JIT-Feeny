package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

public class CallSlotExp implements Exp {
    public final String name;
    public final Exp exp;
    public final Exp[] args;

    public CallSlotExp(String aName, Exp aExp, Exp[] aArgs) {
        name = aName;
        exp = aExp;
        args = aArgs;
    }

    public String toString() {
        return exp + "." + name + "(" + Utils.commas(args) + ")";
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
