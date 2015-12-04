package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.WhileNode;

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
        return new WhileNode(pred.toTruffle(fd), body.toTruffle(fd), fd);
    }
}
