package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

public class ObjectExp implements Exp {
    public Exp parent;
    public final SlotStmt[] slots;

    public ObjectExp(Exp aParent, SlotStmt[] aSlots) {
        parent = aParent;
        slots = aSlots;
    }

    public String toString() {
        return "object(" + parent + ") : (" + Utils.commas(slots) + ")";
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
