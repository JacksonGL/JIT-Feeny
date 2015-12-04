package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

public class SlotExp implements Exp {
    public final String name;
    public final Exp exp;

    public SlotExp(String aName, Exp aExp) {
        name = aName;
        exp = aExp;
    }

    public String toString() {
        return exp + "." + name;
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
