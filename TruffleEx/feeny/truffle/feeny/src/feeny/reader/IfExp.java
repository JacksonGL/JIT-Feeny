package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.IfNode;

public class IfExp implements Exp {
    public final Exp pred;
    public final ScopeStmt conseq;
    public final ScopeStmt alt;

    public IfExp(Exp aPred, ScopeStmt aConseq, ScopeStmt aAlt) {
        pred = aPred;
        conseq = aConseq;
        alt = aAlt;
    }

    public String toString() {
        return "if " + pred + " : (" + conseq + ") else : (" + alt + ")";
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        return new IfNode(pred.toTruffle(fd), conseq.toTruffle(fd), alt.toTruffle(fd), fd);
    }
}
