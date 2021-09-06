#!/usr/bin/env python3

import sys
try:
    import gi
    import re
    import os

    from gi.repository import GLib, Gio

    import unittest
    import socket
    import struct
    import subprocess
    import shutil
    import traceback
    import glob
    import tempfile
except Exception as e:
    print("Missing dependencies: %s" % str(e))
    sys.exit(77)

FPrint = None

# Re-run the test with the passed wrapper if set
wrapper = os.getenv('LIBFPRINT_TEST_WRAPPER')
if wrapper:
    wrap_cmd = wrapper.split(' ') + [sys.executable, os.path.abspath(__file__)] + \
        sys.argv[1:]
    os.unsetenv('LIBFPRINT_TEST_WRAPPER')
    sys.exit(subprocess.check_call(wrap_cmd))

ctx = GLib.main_context_default()


class Connection:

    def __init__(self, addr):
        self.addr = addr

    def __enter__(self):
        self.con = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.con.connect(self.addr)
        return self.con

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.con.close()
        del self.con


class GLibErrorMessage:
    def __init__(self, component, level, expected_message):
        self.level = level
        self.component = component
        self.expected_message = expected_message

    def __enter__(self):
        GLib.test_expect_message(self.component, self.level, self.expected_message)

    def __exit__(self, exc_type, exc_val, exc_tb):
        (filename, line, func_name, text) = traceback.extract_stack()[-2]
        GLib.test_assert_expected_messages_internal(self.component,
            filename, line, func_name)

class VirtualDeviceBase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        unittest.TestCase.setUpClass()
        cls.tmpdir = tempfile.mkdtemp(prefix='libfprint-')

        driver_name = cls.driver_name if hasattr(cls, 'driver_name') else None
        if not driver_name:
            driver_name = re.compile(r'(?<!^)(?=[A-Z])').sub(
                '_', cls.__name__).lower()

        sock_name = driver_name.replace('_', '-')
        cls.sockaddr = os.path.join(cls.tmpdir, '{}.socket'.format(sock_name))
        os.environ['FP_{}'.format(driver_name.upper())] = cls.sockaddr

        cls.ctx = FPrint.Context()

        cls.dev = None
        for dev in cls.ctx.get_devices():
            # We might have a USB device in the test system that needs skipping
            if dev.get_driver() == driver_name:
                cls.dev = dev
                break

        assert cls.dev is not None, "You need to compile with {} for testing".format(driver_name)

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(cls.tmpdir)
        del cls.dev
        del cls.ctx
        unittest.TestCase.tearDownClass()

    def setUp(self):
        super().setUp()
        self._close_on_teardown = True
        self.assertFalse(self.dev.is_open())
        self.dev.open_sync()
        self.assertTrue(self.dev.is_open())

    def tearDown(self):
        if self._close_on_teardown:
            self.assertTrue(self.dev.is_open())
            self.dev.close_sync()
        self.assertFalse(self.dev.is_open())
        super().tearDown()

    def wait_timeout(self, interval):
        timeout_reached = False
        def on_timeout():
            nonlocal timeout_reached
            timeout_reached = True

        GLib.timeout_add(interval, on_timeout)
        while not timeout_reached:
            ctx.iteration(False)

    def send_command(self, command, *args):
        self.assertIn(command, ['INSERT', 'REMOVE', 'SCAN', 'ERROR', 'RETRY',
            'FINGER', 'UNPLUG', 'SLEEP', 'SET_ENROLL_STAGES', 'SET_SCAN_TYPE',
            'SET_CANCELLATION_ENABLED', 'SET_KEEP_ALIVE', 'IGNORED_COMMAND'])

        with Connection(self.sockaddr) as con:
            params = ' '.join(str(p) for p in args)
            con.sendall('{} {}'.format(command, params).encode('utf-8'))

        while ctx.pending():
            ctx.iteration(False)

    def send_finger_report(self, has_finger, iterate=True):
        self.send_command('FINGER', 1 if has_finger else 0)

        if iterate:
            expected = (FPrint.FingerStatusFlags.PRESENT if has_finger
                else ~FPrint.FingerStatusFlags.PRESENT)

            while not (self.dev.get_finger_status() & expected):
                ctx.iteration(True)

    def send_error(self, error):
        self.assertIsInstance(error, FPrint.DeviceError)
        self.send_command('ERROR', int(error))

    def send_retry(self, retry):
        self.assertIsInstance(retry, FPrint.DeviceRetry)
        self.send_command('RETRY', int(retry))

    def send_auto(self, obj):
        if isinstance(obj, FPrint.DeviceError):
            self.send_error(obj)
        elif isinstance(obj, FPrint.DeviceRetry):
            self.send_retry(obj)
        elif isinstance(obj, FPrint.FingerStatusFlags):
            self.send_finger_report(obj & FPrint.FingerStatusFlags.PRESENT, iterate=False)
        elif isinstance(obj, FPrint.ScanType):
            self.send_command('SET_SCAN_TYPE', obj.value_nick)
        elif isinstance(obj, FPrint.Print) and obj.props.fpi_data:
            self.send_command('SCAN', obj.props.fpi_data.unpack())
        else:
            raise Exception('No known type found for {}'.format(obj))

    def set_keep_alive(self, value):
        self.send_command('SET_KEEP_ALIVE', 1 if value else 0)

    def send_sleep(self, interval):
        self.assertGreater(interval, 0)
        multiplier = 5 if 'UNDER_VALGRIND' in os.environ else 1
        self.send_command('SLEEP', interval * multiplier)

    def enroll_print(self, nick, finger, username='testuser', retry_scan=-1):
        self._enrolled = None

        def done_cb(dev, res):
            print("Enroll done")
            try:
                self._enrolled = dev.enroll_finish(res)
            except Exception as e:
                self._enrolled = e

        self._enroll_stage = -1
        def progress_cb(dev, stage, pnt, data, error):
            self._enroll_stage = stage
            self._enroll_progress_error = error

        self.assertLessEqual(retry_scan, self.dev.get_nr_enroll_stages())

        retries = 1
        should_retry = retry_scan > 0

        def enroll_in_progress():
            if self._enroll_stage < 0 and not self._enrolled:
                return True

            if isinstance(self._enrolled, Exception):
                raise(self._enrolled)

            nonlocal retries
            self.assertLessEqual(self._enroll_stage, self.dev.get_nr_enroll_stages())
            if should_retry and retries > retry_scan:
                self.assertEqual(self._enroll_stage, retries - 1)
            else:
                self.assertEqual(self._enroll_stage, retries)

            if retries == retry_scan + 1:
                self.assertIsNotNone(self._enroll_progress_error)
                self.assertEqual(self._enroll_progress_error.code, FPrint.DeviceRetry.TOO_SHORT)
            else:
                self.assertIsNone(self._enroll_progress_error)

            if self._enroll_stage < self.dev.get_nr_enroll_stages():
                self._enroll_stage = -1
                self.assertIsNone(self._enrolled)
                self.assertEqual(self.dev.get_finger_status(),
                    FPrint.FingerStatusFlags.NEEDED)
                if retry_scan == retries:
                    GLib.idle_add(self.send_auto, FPrint.DeviceRetry.TOO_SHORT)
                else:
                    GLib.idle_add(self.send_command, 'SCAN', nick)
                retries += 1

            return not self._enrolled

        self.assertEqual(self.dev.get_finger_status(), FPrint.FingerStatusFlags.NONE)

        self.send_command('SCAN', nick)

        template = FPrint.Print.new(self.dev)
        template.set_finger(finger)
        template.set_username(username)

        self.dev.enroll(template, callback=done_cb, progress_cb=progress_cb)
        while enroll_in_progress():
            ctx.iteration(False)

        self.assertEqual(self._enroll_stage, retries if not should_retry else retries - 1)
        self.assertEqual(self._enroll_stage, self.dev.get_nr_enroll_stages())
        self.assertEqual(self.dev.get_finger_status(), FPrint.FingerStatusFlags.NONE)

        self.assertEqual(self._enrolled.get_device_stored(),
            self.dev.has_storage())
        self.assertEqual(self._enrolled.props.driver, self.dev.get_driver())
        self.assertEqual(self._enrolled.props.device_id, self.dev.get_device_id())
        self.assertEqual(self._enrolled.props.device_stored, self.dev.has_storage())
        self.assertEqual(self._enrolled.props.fpi_data.unpack(), nick)
        self.assertIsNone(self._enrolled.props.image)

        return self._enrolled

    def start_verify(self, p, identify=False):
        self._verify_match = None
        self._verify_fp = None
        self._verify_error = None
        self._verify_report_match = None
        self._verify_report_print = None
        self._verify_completed = False
        self._verify_reported = False
        self._cancellable = Gio.Cancellable()

        if identify:
            self.assertTrue(self.dev.supports_identify())

        def match_cb(dev, match, pnt, data, error):
            self._verify_reported = True
            self._verify_report_match = match
            self._verify_report_print = pnt
            self._verify_report_error = error

        def verify_cb(dev, res):
            try:
                self._verify_match, self._verify_fp = (
                    dev.identify_finish(res) if identify else dev.verify_finish(res))
            except gi.repository.GLib.Error as e:
                self._verify_error = e

            self._verify_completed = True

        if identify:
            self.dev.identify(p if isinstance(p, list) else [p],
                cancellable=self._cancellable, match_cb=match_cb, callback=verify_cb)
        else:
            self.dev.verify(p, cancellable=self._cancellable, match_cb=match_cb,
                callback=verify_cb)

    def cancel_verify(self):
        self._cancellable.cancel()
        while not self._verify_completed:
            ctx.iteration(True)

        self.assertIsNone(self._verify_match)
        self.assertIsNotNone(self._verify_error)
        self.assertEqual(self.dev.get_finger_status(), FPrint.FingerStatusFlags.NONE)

    def complete_verify(self):
        while not self._verify_completed:
            ctx.iteration(True)

        if self._verify_error is not None:
            raise self._verify_error

    def check_verify(self, p, scan_nick, match, identify=False):
        if isinstance(scan_nick, str):
            self.send_command('SCAN', scan_nick)
        elif scan_nick is not None:
            self.send_auto(scan_nick)

        self.start_verify(p, identify)
        self.complete_verify()

        self.assertTrue(self._verify_reported)

        if not match:
            self.assertIsNone(self._verify_report_match)

        if identify:
            if match:
                self.assertIsNotNone(self._verify_report_match)
                self.assertIsNotNone(self._verify_match)
        else:
            if self._verify_fp:
                self.assertEqual(self._verify_fp.equal(p), match)
                if match:
                    self.assertTrue(
                        self._verify_fp.equal(self._verify_report_match))
            else:
                self.assertFalse(match)

        if isinstance(scan_nick, str):
            self.assertEqual(self._verify_fp.props.fpi_data.get_string(), scan_nick)


class VirtualDevice(VirtualDeviceBase):

    def test_device_properties(self):
        self.assertEqual(self.dev.get_driver(), 'virtual_device')
        self.assertEqual(self.dev.get_device_id(), '0')
        self.assertEqual(self.dev.get_name(), 'Virtual device for debugging')
        self.assertTrue(self.dev.is_open())
        self.assertEqual(self.dev.get_scan_type(), FPrint.ScanType.SWIPE)
        self.assertEqual(self.dev.get_nr_enroll_stages(), 5)
        self.assertFalse(self.dev.supports_identify())
        self.assertFalse(self.dev.supports_capture())
        self.assertFalse(self.dev.has_storage())
        self.assertEqual(self.dev.props.driver, self.dev.get_driver())
        self.assertEqual(self.dev.props.device_id, self.dev.get_device_id())
        self.assertEqual(self.dev.props.name, self.dev.get_name())
        self.assertEqual(self.dev.props.scan_type, self.dev.get_scan_type())
        self.assertEqual(self.dev.props.nr_enroll_stages, self.dev.get_nr_enroll_stages())
        self.assertEqual(self.dev.props.open, self.dev.is_open())

    def test_open_error(self):
        self._close_on_teardown = False
        self.send_command('IGNORED_COMMAND') # This will be consumed by close
        self.send_error(FPrint.DeviceError.PROTO) # This will be consumed by open

        with GLibErrorMessage('libfprint-virtual_device',
            GLib.LogLevelFlags.LEVEL_WARNING, 'Could not process command: *'):
            self.dev.close_sync()

        with self.assertRaises(GLib.Error) as error:
            self.dev.open_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.PROTO))

    def test_open_error_with_keep_alive(self):
        self._close_on_teardown = False
        self.set_keep_alive(True)
        self.dev.close_sync()

        self.send_error(FPrint.DeviceError.PROTO)
        with self.assertRaises(GLib.Error) as error:
            self.dev.open_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.PROTO))

    def test_delayed_open(self):
        self.send_command('IGNORED_COMMAND') # This will be consumed by close
        self.send_sleep(500) # This will be consumed by open

        with GLibErrorMessage('libfprint-virtual_device',
            GLib.LogLevelFlags.LEVEL_WARNING, 'Could not process command: *'):
            self.dev.close_sync()

        opened = False
        def on_opened(dev, res):
            nonlocal opened
            dev.open_finish(res)
            opened = True

        self.dev.open(callback=on_opened)

        self.wait_timeout(10)
        self.assertFalse(self.dev.is_open())

        self.wait_timeout(10)
        self.assertFalse(self.dev.is_open())

        while not opened:
            ctx.iteration(True)

    def test_delayed_open_with_keep_alive(self):
        self.set_keep_alive(True)
        self.dev.close_sync()

        opened = False
        def on_opened(dev, res):
            nonlocal opened
            dev.open_finish(res)
            opened = True

        self.send_sleep(500)
        self.dev.open(callback=on_opened)

        self.wait_timeout(10)
        self.assertFalse(self.dev.is_open())

        self.wait_timeout(10)
        self.assertFalse(self.dev.is_open())

        while not opened:
            ctx.iteration(True)

    def test_close_while_opening(self):
        self.set_keep_alive(True)
        self.dev.close_sync()

        opened = False
        def on_opened(dev, res):
            nonlocal opened
            dev.open_finish(res)
            opened = True

        self.send_sleep(500)
        self.dev.open(callback=on_opened)

        self.wait_timeout(10)
        self.assertFalse(self.dev.is_open())

        with self.assertRaises(GLib.Error) as error:
            self.dev.close_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.NOT_OPEN))

        while not opened:
            ctx.iteration(True)

    def test_enroll(self):
        matching = self.enroll_print('testprint', FPrint.Finger.LEFT_LITTLE)
        self.assertEqual(matching.get_username(), 'testuser')
        self.assertEqual(matching.get_finger(), FPrint.Finger.LEFT_LITTLE)

    def test_enroll_with_retry(self):
        matching = self.enroll_print('testprint', FPrint.Finger.LEFT_LITTLE, retry_scan=2)
        self.assertEqual(matching.get_username(), 'testuser')
        self.assertEqual(matching.get_finger(), FPrint.Finger.LEFT_LITTLE)

    def test_enroll_verify_match(self):
        matching = self.enroll_print('testprint', FPrint.Finger.LEFT_THUMB)

        self.check_verify(matching, 'testprint', match=True,
            identify=self.dev.supports_identify())

    def test_enroll_verify_no_match(self):
        matching = self.enroll_print('testprint', FPrint.Finger.LEFT_RING)

        if self.dev.has_storage():
            with self.assertRaises(GLib.Error) as error:
                self.check_verify(matching, 'not-testprint', match=False,
                    identify=self.dev.supports_identify())
            self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                    FPrint.DeviceError.DATA_NOT_FOUND))
        else:
            self.check_verify(matching, 'not-testprint', match=False,
                identify=self.dev.supports_identify())

    def test_enroll_verify_error(self):
        matching = self.enroll_print('testprint', FPrint.Finger.LEFT_RING)

        with self.assertRaises(GLib.Error) as error:
            self.check_verify(matching, FPrint.DeviceError.GENERAL, match=False,
                identify=self.dev.supports_identify())
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.GENERAL))

    def test_enroll_verify_retry(self):
        with self.assertRaises(GLib.GError) as error:
            self.check_verify(FPrint.Print.new(self.dev),
                FPrint.DeviceRetry.TOO_SHORT, match=False)
        self.assertTrue(error.exception.matches(FPrint.DeviceRetry.quark(),
                                                FPrint.DeviceRetry.TOO_SHORT))

    def test_enroll_script_interactive(self):
        enrolled = None
        def done_cb(dev, res):
            nonlocal enrolled
            try:
                enrolled = dev.enroll_finish(res)
            except Exception as e:
                enrolled = e

        enroll_stage = 0
        enroll_progress_error = None
        def progress_cb(dev, stage, pnt, data, error):
            nonlocal enroll_stage, enroll_progress_error
            enroll_stage = stage
            enroll_progress_error = error

        def wait_for_next_stage(expected):
            nonlocal enroll_stage, enroll_progress_error
            enroll_progress_error = None
            next_stage = enroll_stage + 1
            while enroll_stage < next_stage and not enroll_progress_error:
                ctx.iteration(True)

            if isinstance(expected, FPrint.DeviceRetry):
                self.assertEqual(enroll_stage, next_stage - 1)
                self.assertEqual(enroll_progress_error.code, int(expected))
            else:
                self.assertEqual(enroll_stage, expected)
                self.assertIsNone(enroll_progress_error)
            self.assertIsNone(enrolled)

        self.send_sleep(50)
        self.send_command('SCAN', 'print-id')
        self.send_command('SCAN', 'print-id')
        self.send_auto(FPrint.DeviceRetry.TOO_SHORT)
        self.send_command('SCAN', 'print-id')
        self.send_sleep(50)
        self.send_command('SCAN', 'print-id')
        self.send_auto(FPrint.DeviceRetry.CENTER_FINGER)
        self.send_command('SCAN', 'another-id')
        self.send_command('SCAN', 'print-id')

        self.dev.enroll(FPrint.Print.new(self.dev), callback=done_cb,
            progress_cb=progress_cb)

        wait_for_next_stage(1)
        wait_for_next_stage(2)
        wait_for_next_stage(FPrint.DeviceRetry.TOO_SHORT)
        wait_for_next_stage(3)
        wait_for_next_stage(4)
        wait_for_next_stage(FPrint.DeviceRetry.CENTER_FINGER)
        wait_for_next_stage(FPrint.DeviceRetry.GENERAL)
        wait_for_next_stage(5)

        while not enrolled:
            ctx.iteration(True)

        self.assertEqual(enrolled.get_driver(), self.dev.get_driver())
        self.assertEqual(enrolled.props.fpi_data.unpack(), 'print-id')

    def test_enroll_script(self):
        self.send_command('SET_ENROLL_STAGES', 8)
        self.send_command('SCAN', 'print-id')
        self.send_command('SCAN', 'print-id')
        self.send_auto(FPrint.DeviceRetry.TOO_SHORT)
        self.send_command('SCAN', 'print-id')
        self.send_auto(FPrint.DeviceRetry.REMOVE_FINGER)
        self.send_command('SCAN', 'print-id')
        self.send_auto(FPrint.DeviceRetry.CENTER_FINGER)
        self.send_command('SCAN', 'print-id')
        self.send_sleep(10)
        self.send_sleep(20)
        self.send_auto(FPrint.DeviceRetry.GENERAL)
        self.send_auto(FPrint.DeviceRetry.REMOVE_FINGER)
        self.send_command('SCAN', 'print-id')
        self.send_command('SCAN', 'another-id')
        self.send_command('SCAN', 'print-id')
        self.send_command('SCAN', 'print-id')

        enrolled = self.dev.enroll_sync(FPrint.Print.new(self.dev))
        self.assertEqual(enrolled.get_driver(), self.dev.get_driver())
        self.assertEqual(enrolled.props.fpi_data.unpack(), 'print-id')

        return enrolled

    def test_enroll_verify_script(self):
        enrolled = self.test_enroll_script()
        self.send_auto(FPrint.DeviceRetry.CENTER_FINGER)
        with self.assertRaises(GLib.GError) as error:
            self.dev.verify_sync(enrolled)
        self.assertTrue(error.exception.matches(FPrint.DeviceRetry.quark(),
                                                FPrint.DeviceRetry.CENTER_FINGER))

        self.send_sleep(50)
        self.send_auto(FPrint.DeviceRetry.TOO_SHORT)
        with self.assertRaises(GLib.GError) as error:
            self.dev.verify_sync(enrolled)
        self.assertTrue(error.exception.matches(FPrint.DeviceRetry.quark(),
                                                FPrint.DeviceRetry.TOO_SHORT))

        self.send_command('SCAN', 'another-id')
        if self.dev.has_storage():
            with self.assertRaises(GLib.GError) as error:
                self.dev.verify_sync(enrolled)
            self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                    FPrint.DeviceError.DATA_NOT_FOUND))
        else:
            verify_match, verify_fp = self.dev.verify_sync(enrolled)
            self.assertFalse(verify_match)
            self.assertFalse(verify_fp.equal(enrolled))

        self.send_auto(enrolled)
        verify_match, verify_fp = self.dev.verify_sync(enrolled)
        self.assertTrue(verify_match)
        self.assertTrue(verify_fp.equal(enrolled))

    def test_finger_status(self):
        self.start_verify(FPrint.Print.new(self.dev),
            identify=self.dev.supports_identify())

        self.assertEqual(self.dev.get_finger_status(),
                         FPrint.FingerStatusFlags.NEEDED)

        self.send_finger_report(True)
        self.assertEqual(self.dev.get_finger_status(),
            FPrint.FingerStatusFlags.NEEDED | FPrint.FingerStatusFlags.PRESENT)

        self.send_finger_report(False)
        self.assertEqual(self.dev.get_finger_status(), FPrint.FingerStatusFlags.NEEDED)

        self.cancel_verify()

    def test_finger_status_after_sleep(self):
        self.send_sleep(10)
        self.start_verify(FPrint.Print.new(self.dev),
                          identify=self.dev.supports_identify())

        self.assertEqual(self.dev.get_finger_status(),
                         FPrint.FingerStatusFlags.NONE)

        while self.dev.get_finger_status() != FPrint.FingerStatusFlags.NEEDED:
            ctx.iteration(True)

        self.assertEqual(self.dev.get_finger_status(),
                         FPrint.FingerStatusFlags.NEEDED)

        self.send_finger_report(True)
        self.assertEqual(self.dev.get_finger_status(),
                         FPrint.FingerStatusFlags.NEEDED | FPrint.FingerStatusFlags.PRESENT)

        self.send_finger_report(False)
        self.assertEqual(self.dev.get_finger_status(),
                         FPrint.FingerStatusFlags.NEEDED)

        self.cancel_verify()

    def test_change_enroll_stages(self):
        notified_spec = None
        def on_stage_changed(dev, spec):
            nonlocal notified_spec
            notified_spec = spec

        self.dev.connect('notify::nr-enroll-stages', on_stage_changed)

        notified_spec = None
        self.send_command('SET_ENROLL_STAGES', 20)
        self.assertEqual(self.dev.get_nr_enroll_stages(), 20)
        self.assertEqual(notified_spec.name, 'nr-enroll-stages')

        notified_spec = None
        self.send_command('SET_ENROLL_STAGES', 1)
        self.assertEqual(self.dev.get_nr_enroll_stages(), 1)
        self.assertEqual(notified_spec.name, 'nr-enroll-stages')

        with GLibErrorMessage('libfprint-device',
            GLib.LogLevelFlags.LEVEL_CRITICAL, '*enroll_stages > 0*'):
            notified_spec = None
            self.send_command('SET_ENROLL_STAGES', 0)
            self.assertEqual(self.dev.get_nr_enroll_stages(), 1)
            self.assertIsNone(notified_spec)

    def test_quick_enroll(self):
        self.send_command('SET_ENROLL_STAGES', 1)
        self.assertEqual(self.dev.get_nr_enroll_stages(), 1)
        matching = self.enroll_print('testprint', FPrint.Finger.LEFT_LITTLE)
        self.assertEqual(matching.get_username(), 'testuser')
        self.assertEqual(matching.get_finger(), FPrint.Finger.LEFT_LITTLE)

    def test_change_scan_type(self):
        notified_spec = None
        def on_scan_type_changed(dev, spec):
            nonlocal notified_spec
            notified_spec = spec

        self.dev.connect('notify::scan-type', on_scan_type_changed)

        for scan_type in [FPrint.ScanType.PRESS, FPrint.ScanType.SWIPE]:
            notified_spec = None
            self.send_auto(scan_type)
            self.assertEqual(self.dev.get_scan_type(), scan_type)
            self.assertEqual(notified_spec.name, 'scan-type')

        with GLibErrorMessage('libfprint-virtual_device',
            GLib.LogLevelFlags.LEVEL_WARNING, '*Scan type*not found'):
            notified_spec = None
            self.send_command('SET_SCAN_TYPE', 'eye-contact')
            self.assertEqual(self.dev.get_scan_type(), FPrint.ScanType.SWIPE)
            self.assertIsNone(notified_spec)

    def test_device_unplug(self):
        self._close_on_teardown = False
        notified_spec = None
        def on_removed_notify(dev, spec):
            nonlocal notified_spec
            notified_spec = spec

        removed = False
        def on_removed(dev):
            nonlocal removed
            removed = True

        self.assertFalse(self.dev.props.removed)

        self.dev.connect('notify::removed', on_removed_notify)
        self.dev.connect('removed', on_removed)
        self.send_command('UNPLUG')
        self.assertEqual(notified_spec.name, 'removed')
        self.assertTrue(self.dev.props.removed)
        self.assertTrue(removed)

        with self.assertRaises(GLib.GError) as error:
            self.dev.close_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.REMOVED))

    def test_device_unplug_during_verify(self):
        self._close_on_teardown = False

        notified_spec = None
        def on_removed_notify(dev, spec):
            nonlocal notified_spec
            notified_spec = spec

        removed = False
        def on_removed(dev):
            nonlocal removed
            removed = True

        self.assertFalse(self.dev.props.removed)
        self.dev.connect('notify::removed', on_removed_notify)
        self.dev.connect('removed', on_removed)

        self.start_verify(FPrint.Print.new(self.dev),
            identify=self.dev.supports_identify())

        self.send_command('UNPLUG')
        self.assertEqual(notified_spec.name, 'removed')
        self.assertTrue(self.dev.props.removed)
        self.assertFalse(removed)

        with self.assertRaises(GLib.GError) as error:
            self.complete_verify()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.REMOVED))

        self.assertTrue(removed)

        with self.assertRaises(GLib.GError) as error:
            self.dev.close_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.REMOVED))

    def test_device_sleep(self):
        self.send_sleep(1500)

        self.start_verify(FPrint.Print.new(self.dev),
            identify=self.dev.supports_identify())

        self.wait_timeout(300)
        self.assertFalse(self._verify_completed)

        self._cancellable.cancel()
        self.wait_timeout(200)

        self.assertTrue(self._verify_completed)
        self.cancel_verify()

    def test_device_sleep_on_cancellation(self):
        self.send_command('SET_CANCELLATION_ENABLED', int(False))
        self.send_sleep(1500)
        self.send_command('SCAN', 'foo-print')

        self.start_verify(FPrint.Print.new(self.dev),
            identify=self.dev.supports_identify())
        self.wait_timeout(300)

        self.assertFalse(self._verify_completed)

        self._cancellable.cancel()
        self.wait_timeout(300)

        self.assertFalse(self._verify_completed)
        self.cancel_verify()

        # Since we don't really cancel here, next command will be passed to release
        self._close_on_teardown = False
        with GLibErrorMessage('libfprint-virtual_device',
            GLib.LogLevelFlags.LEVEL_WARNING, 'Could not process command: SCAN *'):
            self.dev.close_sync()

    def test_device_sleep_before_completing_verify(self):
        enrolled = self.enroll_print('foo-print', FPrint.Finger.LEFT_RING)

        self.send_sleep(100)
        self.start_verify(enrolled, identify=self.dev.supports_identify())
        self.send_command('SCAN', 'bar-print')
        self.send_sleep(800)

        while not self._verify_reported:
            ctx.iteration(False)

        self.assertFalse(self._verify_completed)
        self.wait_timeout(10)
        self.assertFalse(self._verify_completed)

        if self.dev.has_storage():
            with self.assertRaises(GLib.Error) as error:
                self.complete_verify()
            self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                    FPrint.DeviceError.DATA_NOT_FOUND))
        else:
            self.complete_verify()
        self.assertTrue(self._verify_reported)

    def test_close_error(self):
        self._close_on_teardown = False
        close_res = None

        def on_closed(dev, res):
            nonlocal close_res
            try:
                close_res = dev.close_finish(res)
            except GLib.Error as e:
                close_res = e

        self.send_sleep(100)
        self.send_error(FPrint.DeviceError.BUSY)
        self.dev.close(callback=on_closed)
        self.wait_timeout(2)
        self.assertIsNone(close_res)

        while not close_res:
            ctx.iteration(True)

        self.assertEqual(close_res.code, int(FPrint.DeviceError.BUSY))


class VirtualDeviceClosed(VirtualDeviceBase):

    driver_name = 'virtual_device'

    def setUp(self):
        super().setUp()
        self._close_on_teardown = False
        self.dev.close_sync()
        self.assertFalse(self.dev.is_open())

    def test_close(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.close_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.NOT_OPEN))

    def test_enroll(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.enroll_sync(FPrint.Print.new(self.dev))
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.NOT_OPEN))

    def test_verify(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.verify_sync(FPrint.Print.new(self.dev))
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.NOT_OPEN))

    def test_identify(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.identify_sync([FPrint.Print.new(self.dev)])
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.NOT_OPEN))

    def test_capture(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.capture_sync(wait_for_finger=False)
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.NOT_OPEN))

    def test_delete_print(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.delete_print_sync(FPrint.Print.new(self.dev))
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.NOT_OPEN))

    def test_list_prints(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.list_prints_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.NOT_OPEN))


class VirtualDeviceBusyDeviceOperations(VirtualDeviceBase):

    driver_name = 'virtual_device'

    def setUp(self):
        super().setUp()
        self._close_on_teardown = False
        self.send_sleep(200)
        self.dev.close()

    def tearDown(self):
        while self.dev.is_open():
            ctx.iteration(True)
        super().tearDown()

    def test_open(self):
        self.set_keep_alive(True)

        while self.dev.is_open():
            ctx.iteration(True)

        self.assertFalse(self.dev.is_open())
        self.send_sleep(100)
        self.dev.open()
        with self.assertRaises(GLib.Error) as error:
            self.dev.open_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.BUSY))

        self.assertFalse(self.dev.is_open())
        while not self.dev.is_open():
            ctx.iteration(True)

        self.dev.close_sync()

    def test_close(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.close_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.BUSY))

    def test_enroll(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.enroll_sync(FPrint.Print.new(self.dev))
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.BUSY))

    def test_verify(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.verify_sync(FPrint.Print.new(self.dev))
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.BUSY))

    def test_identify(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.identify_sync([FPrint.Print.new(self.dev)])
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.BUSY))

    def test_capture(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.capture_sync(wait_for_finger=False)
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.BUSY))

    def test_delete_print(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.delete_print_sync(FPrint.Print.new(self.dev))
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.BUSY))

    def test_list_prints(self):
        with self.assertRaises(GLib.Error) as error:
            self.dev.list_prints_sync()
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.BUSY))


class VirtualDeviceStorage(VirtualDevice):

    def tearDown(self):
        self.cleanup_device_storage()
        super().tearDown()

    def cleanup_device_storage(self):
        if self.dev.is_open() and not self.dev.props.removed:
            for print in self.dev.list_prints_sync():
                self.assertTrue(self.dev.delete_print_sync(print, None))

    def test_device_properties(self):
        self.assertEqual(self.dev.get_driver(), 'virtual_device_storage')
        self.assertEqual(self.dev.get_device_id(), '0')
        self.assertEqual(self.dev.get_name(),
            'Virtual device with storage and identification for debugging')
        self.assertTrue(self.dev.is_open())
        self.assertEqual(self.dev.get_scan_type(), FPrint.ScanType.SWIPE)
        self.assertEqual(self.dev.get_nr_enroll_stages(), 5)
        self.assertTrue(self.dev.supports_identify())
        self.assertFalse(self.dev.supports_capture())
        self.assertTrue(self.dev.has_storage())

    def test_duplicate_enroll(self):
        self.enroll_print('testprint', FPrint.Finger.LEFT_LITTLE)
        with self.assertRaises(GLib.Error) as error:
            self.enroll_print('testprint', FPrint.Finger.LEFT_LITTLE)
            self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                    FPrint.DeviceError.DATA_DUPLICATE))

    def test_list_empty(self):
        self.assertFalse(self.dev.list_prints_sync())

    def test_list_populated(self):
        self.send_command('INSERT', 'p1')
        print2 = self.enroll_print('p2', FPrint.Finger.LEFT_LITTLE)
        self.assertEqual({'p1', 'p2'}, {p.props.fpi_data.get_string() for p in self.dev.list_prints_sync()})

    def test_list_delete(self):
        p = self.enroll_print('testprint', FPrint.Finger.RIGHT_THUMB)
        l = self.dev.list_prints_sync()
        print(l[0])
        self.assertEqual(len(l), 1)
        print('blub', p.props.fpi_data, type(l[0].props.fpi_data))
        assert p.equal(l[0])
        self.dev.delete_print_sync(p)
        self.assertFalse(self.dev.list_prints_sync())

    def test_delete_error(self):
        deleted_res = None
        def on_deleted(dev, res):
            nonlocal deleted_res
            try:
                deleted_res = dev.delete_print_finish(res)
            except GLib.Error as e:
                deleted_res = e

        self.send_sleep(100)
        self.send_error(FPrint.DeviceError.DATA_NOT_FOUND)
        self.dev.delete_print(FPrint.Print.new(self.dev), callback=on_deleted)
        self.wait_timeout(2)
        self.assertIsNone(deleted_res)

        while not deleted_res:
            ctx.iteration(True)

        self.assertEqual(deleted_res.code, int(FPrint.DeviceError.DATA_NOT_FOUND))

    def test_list_error(self):
        list_res = None

        def on_listed(dev, res):
            nonlocal list_res
            try:
                list_res = dev.list_prints_finish(res)
            except GLib.Error as e:
                list_res = e

        self.send_sleep(100)
        self.send_error(FPrint.DeviceError.BUSY)
        self.dev.list_prints(callback=on_listed)
        self.wait_timeout(2)
        self.assertIsNone(list_res)

        while not list_res:
            ctx.iteration(True)

        self.assertEqual(list_res.code, int(FPrint.DeviceError.BUSY))

    def test_list_delete_missing(self):
        p = self.enroll_print('testprint', FPrint.Finger.RIGHT_THUMB)
        self.send_command('REMOVE', 'testprint')

        with self.assertRaises(GLib.Error) as error:
            self.dev.delete_print_sync(p)
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.DATA_NOT_FOUND))

    def test_identify_match(self):
        rt = self.enroll_print('right-thumb', FPrint.Finger.RIGHT_THUMB)
        lt = self.enroll_print('left-thumb', FPrint.Finger.LEFT_THUMB)

        self.check_verify([rt, lt], 'right-thumb', identify=True, match=True)
        self.check_verify([rt, lt], 'left-thumb', identify=True, match=True)

    def test_identify_no_match(self):
        rt = self.enroll_print('right-thumb', FPrint.Finger.RIGHT_THUMB)
        lt = self.enroll_print('left-thumb', FPrint.Finger.LEFT_THUMB)

        self.check_verify(lt, 'right-thumb', identify=True, match=False)
        self.check_verify(rt, 'left-thumb', identify=True, match=False)

    def test_identify_retry(self):
        with self.assertRaises(GLib.GError) as error:
            self.check_verify(FPrint.Print.new(self.dev),
                FPrint.DeviceRetry.TOO_SHORT, identify=True, match=False)
        self.assertTrue(error.exception.matches(FPrint.DeviceRetry.quark(),
                                                FPrint.DeviceRetry.TOO_SHORT))

    def test_delete_multiple_times(self):
        rt = self.enroll_print('right-thumb', FPrint.Finger.RIGHT_THUMB)
        self.dev.delete_print_sync(rt)

        with self.assertRaises(GLib.Error) as error:
            self.dev.delete_print_sync(rt)
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.DATA_NOT_FOUND))

    def test_verify_missing_print(self):
        with self.assertRaises(GLib.Error) as error:
            self.check_verify(FPrint.Print.new(self.dev),
                'not-existing-print', False, identify=False)
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.DATA_NOT_FOUND))

    def test_identify_missing_print(self):
        with self.assertRaises(GLib.Error) as error:
            self.check_verify(FPrint.Print.new(self.dev),
                              'not-existing-print', False, identify=True)
        self.assertTrue(error.exception.matches(FPrint.DeviceError.quark(),
                                                FPrint.DeviceError.DATA_NOT_FOUND))


if __name__ == '__main__':
    try:
        gi.require_version('FPrint', '2.0')
        from gi.repository import FPrint
    except Exception as e:
        print("Missing dependencies: %s" % str(e))
        sys.exit(77)

    # avoid writing to stderr
    unittest.main(testRunner=unittest.TextTestRunner(stream=sys.stdout, verbosity=2))
