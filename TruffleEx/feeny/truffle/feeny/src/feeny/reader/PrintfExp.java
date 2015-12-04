package feeny.reader;

import java.util.ArrayList;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.PrintfNode;

public class PrintfExp implements Exp {
    public final String format;
    public final Exp[] exps;

    public PrintfExp(String aFormat, Exp[] aExps) {
        format = aFormat;
        exps = aExps;
    }

    public String toString() {
        if (exps.length == 0) {
            return "printf(" + format + ")";
        } else {
            return "printf(" + format + ", " + Utils.commas(exps) + ")";
        }
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        RootNode[] nodes = new RootNode[exps.length];
        for (int i = 0; i < exps.length; i++) {
            nodes[i] = exps[i].toTruffle(fd);
        }
        return new PrintfNode(format, nodes, fd);
    }
}
