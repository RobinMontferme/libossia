#include "Network/Protocol/JamomaMIDI.h"
using namespace OSSIA;

# pragma mark -
# pragma mark Life Cycle

shared_ptr<MIDI> MIDI::create()
{
  return make_shared<JamomaMIDI>();
}

JamomaMIDI::JamomaMIDI():
    mInput{"ossia-in"},
    mOutput{"ossia-out"}
{}

JamomaMIDI::~JamomaMIDI()
{
    mInput.closePort();
    mOutput.closePort();
}

MIDI::~MIDI()
{}

bool JamomaMIDI::setInfo(MidiInfo m)
{
    // Close current ports
    if(mInfo.type == MidiInfo::Type::RemoteOutput)
    {
        mInput.closePort();
    }
    else if(mInfo.type == MidiInfo::Type::RemoteInput)
    {
        mOutput.closePort();
    }

    mInfo = m;

    if(mInfo.type == MidiInfo::Type::RemoteOutput)
    {
        mInput.openPort(mInfo.port);
        mInput.messageCallback = [=] (mm::MidiMessage mess) {
            Channel& c = mChannels[mess.getChannel()];
            switch(mess.getMessageType())
            {
                case mm::MessageType::NOTE_ON:
                    c.mNoteOn.first = mess.data[1];
                    c.mNoteOn.second = mess.data[2];
                    c.mNoteOn_N[c.mNoteOn.first] = c.mNoteOn.second;
                    break;
                case mm::MessageType::NOTE_OFF:
                    c.mNoteOff.first = mess.data[1];
                    c.mNoteOff.second = mess.data[2];
                    c.mNoteOff_N[c.mNoteOff.first] = c.mNoteOff.second;
                    break;
                case mm::MessageType::CONTROL_CHANGE:
                    c.mCC.first = mess.data[1];
                    c.mCC.second = mess.data[2];
                    c.mCC_N[c.mCC.first] = c.mCC.second;
                    break;
            }
        };
    }
    else if(mInfo.type == MidiInfo::Type::RemoteInput)
    {
        mOutput.openPort(mInfo.port);
    }
}

MidiInfo JamomaMIDI::getInfo() const
{
    return mInfo;
}

# pragma mark -
# pragma mark Operation

bool JamomaMIDI::pullAddressValue(Address& address) const
{
  MIDIAddress& adrs = dynamic_cast<MIDIAddress&>(address);
  if(mInfo.type != MidiInfo::Type::RemoteOutput)
      return false;

  auto& adrinfo = adrs.info();
  const Channel& chan = mChannels[adrinfo.channel];
  switch(adrinfo.type)
  {
      case MIDIAddressInfo::Type::NoteOn_N:
      {
          OSSIA::Int val{chan.mNoteOn_N[adrinfo.note]};
          address.setValue(&val);
          return true;
      }

      case MIDIAddressInfo::Type::NoteOn:
      {
          OSSIA::Tuple val{OSSIA::Tuple::ValueInit{},
              OSSIA::Int{chan.mNoteOn.first},
              OSSIA::Int{chan.mNoteOn.second}};
          address.setValue(&val);
          return true;
      }

      case MIDIAddressInfo::Type::NoteOff_N:
      {
          OSSIA::Int val{chan.mNoteOff_N[adrinfo.note]};
          address.setValue(&val);
          return true;
      }

      case MIDIAddressInfo::Type::NoteOff:
      {
          OSSIA::Tuple val{OSSIA::Tuple::ValueInit{},
              OSSIA::Int{chan.mNoteOff.first},
              OSSIA::Int{chan.mNoteOff.second}};
          address.setValue(&val);
          return true;
      }

      case MIDIAddressInfo::Type::CC_N:
      {
          OSSIA::Int val{chan.mCC_N[adrinfo.note]};
          address.setValue(&val);
          return true;
      }

      case MIDIAddressInfo::Type::CC:
      {
          OSSIA::Tuple val{OSSIA::Tuple::ValueInit{},
              OSSIA::Int{chan.mCC.first},
              OSSIA::Int{chan.mCC.second}};
          address.setValue(&val);
          return true;
      }
      default:
          return false;
  }

}

bool JamomaMIDI::pushAddressValue(const Address& address) const
{
    const MIDIAddress& adrs = dynamic_cast<const MIDIAddress&>(address);
    if(mInfo.type != MidiInfo::Type::RemoteInput)
        return false;

    auto& adrinfo = adrs.info();
    switch(adrinfo.type)
    {
        case MIDIAddressInfo::Type::NoteOn_N:
        {
            mOutput.send(mm::MakeNoteOn(
                             adrinfo.channel,
                             adrinfo.note,
                             static_cast<const OSSIA::Int*>(address.getValue())->value));
            return true;
        }

        case MIDIAddressInfo::Type::NoteOn:
        {
            auto val = static_cast<const OSSIA::Tuple*>(address.getValue());
            mOutput.send(mm::MakeNoteOn(
                             adrinfo.channel,
                             static_cast<const OSSIA::Int*>(val->value[0])->value,
                             static_cast<const OSSIA::Int*>(val->value[1])->value));
            return true;
        }

        case MIDIAddressInfo::Type::NoteOff_N:
        {
            mOutput.send(mm::MakeNoteOff(
                             adrinfo.channel,
                             adrinfo.note,
                             static_cast<const OSSIA::Int*>(address.getValue())->value));
            return true;
        }

        case MIDIAddressInfo::Type::NoteOff:
        {
            auto val = static_cast<const OSSIA::Tuple*>(address.getValue());
            mOutput.send(mm::MakeNoteOff(
                             adrinfo.channel,
                             static_cast<const OSSIA::Int*>(val->value[0])->value,
                             static_cast<const OSSIA::Int*>(val->value[1])->value));
            return true;
        }

        case MIDIAddressInfo::Type::CC_N:
        {
            mOutput.send(mm::MakeControlChange(
                             adrinfo.channel,
                             adrinfo.note,
                             static_cast<const OSSIA::Int*>(address.getValue())->value));
            return true;
        }

        case MIDIAddressInfo::Type::CC:
        {
            auto val = static_cast<const OSSIA::Tuple*>(address.getValue());
            mOutput.send(mm::MakeControlChange(
                             adrinfo.channel,
                             static_cast<const OSSIA::Int*>(val->value[0])->value,
                             static_cast<const OSSIA::Int*>(val->value[1])->value));
            return true;
        }
        default:
            return false;
    }

    return true;
}

bool JamomaMIDI::observeAddressValue(std::shared_ptr<Address> address, bool enable) const
{
  return true;
}

bool JamomaMIDI::updateChildren(Node& node) const
{
  return false;
}

vector<MidiInfo> JamomaMIDI::scan()
{
     std::vector<MidiInfo> vec;

     {
         // Input devices are those on which we do output
         auto& in = mInput;
         auto dev = in.getInputDevice();
         auto portcount = dev->getPortCount();
         for(int i = 0; i < portcount; i++)
         {
             vec.emplace_back(MidiInfo::Type::RemoteOutput, dev->getPortName(i), i);
         }
     }

     {
         // Output devices are those that will send data to us
         auto& out = mOutput;
         auto dev = out.getOutputDevice();
         auto portcount = dev->getPortCount();
         for(int i = 0; i < portcount; i++)
         {
             vec.emplace_back(MidiInfo::Type::RemoteInput, dev->getPortName(i), i);
         }
     }

     return vec;
}

std::shared_ptr<OSSIA::Device> OSSIA::createMIDIDevice(std::shared_ptr<MIDI> mid)
{
    return std::make_shared<MIDIDeviceImpl>(mid);
}
