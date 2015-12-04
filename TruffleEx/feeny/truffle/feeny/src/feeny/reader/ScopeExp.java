package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

public class ScopeExp implements ScopeStmt {
    public final Exp exp;

    public ScopeExp(Exp aExp) {
        exp = aExp;
    }

    public String toString() {
        return exp.toString();
    }

    public RootNode toTruffle(FrameDescriptor fd) {
        return exp.toTruffle(fd);
    }
}
