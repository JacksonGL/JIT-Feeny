package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;
import feeny.nodes.CallSlotNode;

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
        RootNode[] args_n = new RootNode[args.length];
        for (int i = 0; i < args.length; ++i) {
            args_n[i] = args[i].toTruffle(fd);
        }
        return new CallSlotNode(name, exp.toTruffle(fd), args_n, fd);
    }
}
