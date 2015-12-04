package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.ScopeSeqNode;

public class ScopeSeq implements ScopeStmt {
    public final ScopeStmt a;
    public final ScopeStmt b;

    public ScopeSeq(ScopeStmt sa, ScopeStmt sb) {
        a = sa;
        b = sb;
    }

    public String toString() {
        return a + " " + b;
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        return new ScopeSeqNode(fd, a, b);
    }
}
