package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

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
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
