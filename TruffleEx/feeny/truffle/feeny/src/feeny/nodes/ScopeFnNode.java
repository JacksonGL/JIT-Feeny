package feeny.nodes;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;
import feeny.reader.ScopeStmt;

public class ScopeFnNode extends RootNode {
    String args[];
    @Child RootNode body;
    String name;
    FrameSlot slot;

    public ScopeFnNode(String name, String[] args2, ScopeStmt body2, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        this.args = args2;
        FrameDescriptor fd2 = new FrameDescriptor(frameDescriptor);
        for (int i = 0; i < args.length; i++) {
            fd2.addFrameSlot(args[i]);
        }
        this.body = body2.toTruffle(fd2);
        this.name = name;
        this.slot = frameDescriptor.addFrameSlot(name, FrameSlotKind.Object);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        frame.setObject(slot, this);
        return null;
    }

    public String[] getArgs() {
        return args;
    }

    public void setArgs(String[] args) {
        this.args = args;
    }

    public RootNode getBody() {
        return body;
    }

    public void setBody(RootNode body) {
        this.body = body;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
