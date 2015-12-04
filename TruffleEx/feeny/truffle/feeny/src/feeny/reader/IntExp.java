package feeny.reader;

import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.IntNode;

public class IntExp implements Exp {
    public final int value;

    public IntExp(int aValue) {
        value = aValue;
    }

    public String toString() {
        return Integer.toString(value);
    }

    @SuppressWarnings("cast")
    public RootNode toTruffle(FrameDescriptor fd) {
        return (RootNode) new IntNode(value, fd);
    }
}
