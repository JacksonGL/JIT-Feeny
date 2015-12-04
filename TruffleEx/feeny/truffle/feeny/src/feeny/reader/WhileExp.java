package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

public class WhileExp implements Exp {
    public final Exp pred;
    public final ScopeStmt body;

    public WhileExp(Exp aPred, ScopeStmt aBody) {
        pred = aPred;
        body = aBody;
    }

    public String toString() {
        return "while " + pred + " : (" + body + ")";
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
