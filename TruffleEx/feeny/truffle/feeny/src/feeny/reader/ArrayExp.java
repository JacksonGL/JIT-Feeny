package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

public class ArrayExp implements Exp {
    public final Exp length;
    public final Exp init;

    public ArrayExp(Exp aLength, Exp aInit) {
        length = aLength;
        init = aInit;
    }

    public String toString() {
        return "array(" + length + ", " + init + ")";
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
