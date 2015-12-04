package feeny.reader;

import com.oracle.truffle.api.nodes.RootNode;

public class ScopeExp implements ScopeStmt {
    public final Exp exp;

    public ScopeExp(Exp aExp) {
        exp = aExp;
    }

    public String toString() {
        return exp.toString();
    }

    public RootNode toTruffle() {
        throw new UnsupportedOperationException("ScopeExp is not supported yet");
    }
}
