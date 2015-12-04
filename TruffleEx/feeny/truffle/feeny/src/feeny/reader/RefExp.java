package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.RefExpNode;

public class RefExp implements Exp {
    public final String name;

    public RefExp(String aName) {
        name = aName;
    }

    public String toString() {
        return name;
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        return new RefExpNode(name, fd);
    }
}
