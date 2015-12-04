package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

public interface ScopeStmt {
    public RootNode toTruffle(FrameDescriptor fd);
}
