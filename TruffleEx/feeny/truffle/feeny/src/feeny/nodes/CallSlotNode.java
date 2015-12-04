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

public class CallSlotNode extends RootNode {
    final String name;
    @Child RootNode reciever;
    @Children final RootNode[] args;

    public CallSlotNode(String name_, RootNode receiver_, RootNode[] args_, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        name = name_;
        reciever = receiver_;
        args = args_;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        System.out.println("Evaluating " + this.getClass().getName() + ":" + name);
        if (name.equals("add")) {
            return ((Integer) reciever.execute(frame)) + ((Integer) args[0].execute(frame));
        } else if (name.equals("sub")) {
            return ((Integer) reciever.execute(frame)) - ((Integer) args[0].execute(frame));
        } else if (name.equals("lt")) {
            return ((Integer) reciever.execute(frame)) < ((Integer) args[0].execute(frame)) ? new Integer(0) : null;
        } else {
            return null;
        }
    }
}
