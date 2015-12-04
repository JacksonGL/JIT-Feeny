package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.CallNode;

public class CallExp implements Exp {
    public final String name;
    public final Exp[] args;

    public CallExp(String aName, Exp[] aArgs) {
        name = aName;
        args = aArgs;
    }

    public String toString() {
        return name + "(" + Utils.commas(args) + ")";
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        RootNode[] args_n = new RootNode[args.length];
        for (int i = 0; i < args.length; ++i) {
            args_n[i] = args[i].toTruffle(fd);
        }
        return new CallNode(name, args_n, fd);
    }
}
