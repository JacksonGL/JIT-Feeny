package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

public class RefExp implements Exp {
    public final String name;

    public RefExp(String aName) {
        name = aName;
    }

    public String toString() {
        return name;
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
