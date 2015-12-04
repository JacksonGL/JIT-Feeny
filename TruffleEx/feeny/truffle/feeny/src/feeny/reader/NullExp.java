package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.NullNode;

public class NullExp implements Exp {
    public String toString() {
        return "null";
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        return new NullNode(fd);
    }
}
